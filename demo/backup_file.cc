#include <deuceclient/deuceclient.h>
#include <boost/foreach.hpp>

#include <fcntl.h>
#if defined(WIN32)
#include <sys/stat.h>
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

#include "demo_helpers.h"

#include <thread>
#include <mutex>
#include <condition_variable>

using std::thread;
using std::unique_lock;
using std::mutex;
using std::condition_variable;

using namespace rax;

std::string backup_file(char const* filename);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "usage: ./backup_file <PATH>\n";
		exit(1);
	}

	try
	{
		auto fileid = backup_file(argv[1]);

		std::cout << "fileid = " << fileid << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

template <typename Upload>
struct bundle_uploader
{
	explicit bundle_uploader(Upload f) :
		q_(1),
		i_(0),
		upload_(std::move(f)),
		t_(&bundle_uploader::worker, this)
	{
		t_.detach();
	}

	template <typename Range>
	void copy_missing_blocks_from(Range const& ids,
	    deuceclient::bundle const& bs)
	{
		auto it = bs.blocks().begin();
		auto ed = bs.blocks().end();

		unique_lock<mutex> lock(m_);
		not_uploading_.wait(lock, [&]{ return i_ == 0; });

		BOOST_FOREACH(auto&& id, ids)
		{
			it = std::find_if(it, ed,
			    [&](decltype(*it) binfo)
			    {
				return std::get<1>(binfo) == id;
			    });

			if (it == ed)
				break;

			if (not suites_for_block(it, bs))
			{
				++i_;
				if (i_ == q_.size())
					q_.emplace_back(5 * 1024 * 1024);
			}

			bs.copy_block(it, q_[i_]);
		}

		if (i_ > 0)
			not_empty_.notify_one();
	}

	void join()
	{
		unique_lock<mutex> lock(m_);
		not_uploading_.wait(lock, [&]{ return i_ == 0; });

		if (q_[i_].size() > 0)
			upload_(q_[i_]);
	}

private:
	template <typename Iter>
	bool suites_for_block(Iter it, deuceclient::bundle const& bs) const
	{
		return bs.size_of_block(it) <=
		    q_[i_].capacity() - q_[i_].size() and
		    bs.serialized_size_of_block(it) <=
		    10 * 1024 * 1024 - q_[i_].serialized_size();
	}

	void worker()
	{
		while (1)
		{
			unique_lock<mutex> lock(m_);
			not_empty_.wait(lock, [&]{ return i_ > 0; });

			std::for_each(begin(q_), begin(q_) + i_, upload_);

			std::swap(q_[0], q_[i_]);
			i_ = 0;

			not_uploading_.notify_one();
		}
	}

	std::vector<deuceclient::bundle> q_;
	size_t i_;
	Upload upload_;
	thread t_;
	mutex m_;
	condition_variable not_empty_;
	condition_variable not_uploading_;
};

std::string backup_file(char const* filename)
{
#if defined(WIN32)
	int fd;
	_sopen_s(&fd, filename, _O_RDONLY | _O_BINARY , _SH_DENYWR, _S_IREAD);
#else
	auto fd = open(filename, O_RDONLY);
#endif

	if (fd == -1)
		THROW_ERRNO();

#if defined(WIN32)
	defer(_close(fd));
#else
	defer(close(fd));
#endif

	auto client = deuceclient::client(getenv_or("DEUCE_HOST",
	    "http://localhost:8080"), "demo_project");
	auto vault = client.create_vault("demo");

	auto fupload = [&](deuceclient::bundle& bn)
	    {
		vault.upload_bundle(bn);
	    };

	deuceclient::managed_bundle<rabin_boundary> bs(15 * 1024 * 1024);
	deuceclient::block_arrangement ba;
	bundle_uploader<decltype(fupload)> bu(fupload);

	bs.boundary().set_limits(14843, 17432, 90406);

	int64_t file_size = 0;
	bool bundle_is_full;

	auto f = vault.make_file();
	defer(vault.delete_file(f.id())) namely(delete_file);

	do
	{
		bundle_is_full = bs.consume(
		    [=](char* p, size_t sz)
		    {
#if defined(WIN32)
			return _read(fd, p, unsigned(sz));
#else
			return read(fd, p, sz);
#endif
		    });

		if (bs.size() == 0)
			break;

		int64_t offset = file_size;

		BOOST_FOREACH(auto&& t, bs.blocks())
		{
			size_t end_of_block;
			deuceclient::sha1_digest blockid;
			std::tie(end_of_block, blockid) = t;

			ba.add(blockid, offset);
			offset = file_size + end_of_block;
		}

		bu.copy_missing_blocks_from(f.assign_blocks(ba), bs);
		file_size += bs.size();
		bs.clear();

	} while (bundle_is_full);

	bu.join();

	f.finalize_file(file_size);
	delete_file.dismiss();

	return f.id().to_string();
}

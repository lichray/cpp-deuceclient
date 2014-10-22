#include <deuceclient/deuceclient.h>
#include <boost/foreach.hpp>

#include <cstdlib>
#include <iostream>

#include <fcntl.h>
#if defined(WIN32)
#include <sys/stat.h>
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

#include "defer.h"

#if !(defined(_MSC_VER) && _MSC_VER < 1700)
#define THROW_ERRNO() throw std::system_error(errno, std::system_category())
#else
#define THROW_ERRNO() do {						\
	std::error_code ec(errno, std::system_category());		\
	throw std::system_error(ec, ec.message());			\
} while(0)
#endif

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

	auto client = deuceclient::client("http://localhost:8080",
	    "demo_project");
	auto vault = client.create_vault("demo");

	deuceclient::managed_bundle<rabin_boundary> bs;
	deuceclient::bundle bn;
	deuceclient::block_arrangement ba;

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
		size_t nblocks = 0;
		auto it = bs.blocks().begin();

		auto send_to_dedup = [&]()
		    {
			BOOST_FOREACH(auto&& id, f.assign_blocks(ba))
			{
				it = std::find_if(it, bs.blocks().end(),
				    [&](decltype(*it) binfo)
				    {
					return std::get<1>(binfo) == id;
				    });

				if (it == bs.blocks().end())
					break;

				// keep the request body under 10MB
				if (bs.size_of_block(it) >
				    bn.max_size() - bn.size() or
				    bs.serialized_size_of_block(it) >
				    10 * 1024 * 1024 - bn.serialized_size())
					vault.upload_bundle(bn);

				bs.copy_block(it, bn);
			}
		    };

		BOOST_FOREACH(auto&& t, bs.blocks())
		{
			size_t end_of_block;
			deuceclient::sha1_digest blockid;
			std::tie(end_of_block, blockid) = t;

			ba.add(blockid, offset);
			++nblocks;
			offset = file_size + end_of_block;

			// keep the payload under 64KB
			if (nblocks == 1000)
			{
				send_to_dedup();
				nblocks = 0;
			}
		}

		if (nblocks > 0)
			send_to_dedup();

		file_size += bs.size();
		bs.clear();

	} while (bundle_is_full);

	if (bn.size() > 0)
		vault.upload_bundle(bn);

	f.finalize_file(file_size);
	delete_file.dismiss();

	return f.id().to_string();
}

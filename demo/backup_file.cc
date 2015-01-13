#include <fcntl.h>
#if defined(WIN32)
#include <sys/stat.h>
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

#include "demo_helpers.h"

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

	auto client = make_demo_client();
	auto vault = client.create_vault("demo");

	deuceclient::managed_bundle<rabin_boundary> bs(15 * 1024 * 1024);
	bs.boundary().set_limits(14843, 17432, 90406);

	auto fu = deuceclient::make_file_uploader(
	    [&](deuceclient::bundle& bn)
	    {
		vault.upload_bundle(bn);
	    });

	auto f = vault.make_file();
	defer(vault.delete_file(f.id())) namely(delete_file);

	auto file_size = fu.consume_all(f, bs,
	    [=](char* p, size_t sz)
	    {
#if defined(WIN32)
		return _read(fd, p, unsigned(sz));
#else
		return read(fd, p, sz);
#endif
	    });

	fu.finish();

	f.finalize_file(file_size);
	delete_file.dismiss();

	return f.id().to_string();
}

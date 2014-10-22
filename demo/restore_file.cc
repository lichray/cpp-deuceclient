#include <deuceclient/deuceclient.h>
#include <httpverbs/c_file.h>

#include <cstdlib>
#include <iostream>
#include <cstdio>

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
using namespace httpverbs::keywords;

void restore_file(char const* fileid);

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "usage: ./restore_file <PATH> <fileid>\n";
		exit(1);
	}

	try
	{
		restore_file(argv[2]);

#if defined(WIN32)
		_unlink(argv[1]);
		errno = 0;
#endif
		if (rename(argv[2], argv[1]) == -1)
			THROW_ERRNO();
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void restore_file(char const* fileid)
{
#if !defined(_MSC_VER)
	auto fp = fopen(fileid, "wb");
#else
	FILE* fp;
	fopen_s(&fp, fileid, "wb");
#endif

	if (fp == nullptr)
		THROW_ERRNO();

	defer(remove(fileid)) namely(delete_temp);
	defer(fclose(fp));

	auto client = deuceclient::client("http://localhost:8080",
	    "demo_project");
	auto vault = client.get_vault("demo");

	vault.download_file(fileid, to_c_file(fp));
	delete_temp.dismiss();
}

#include <deuceclient/deuceclient.h>
#include <httpverbs/c_file.h>

#include <cstdlib>
#include <iostream>
#include <cstdio>

#include "defer.h"

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

		if (rename(argv[2], argv[1]) == -1)
			throw std::system_error(errno, std::system_category());
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void restore_file(char const* fileid)
{
	auto fp = fopen(fileid, "w");
	defer(remove(fileid)) namely(delete_temp);
	defer(fclose(fp));

	if (fp == nullptr)
		throw std::system_error(errno, std::system_category());

	auto client = deuceclient::client("http://localhost:8080",
	    "demo_project");
	auto vault = client.get_vault("demo");

	vault.download_file(fileid, to_c_file(fp));
	delete_temp.dismiss();
}

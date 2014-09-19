/*
 * Copyright 2014 Rackspace, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RAX_DEUCECLIENT_CLIENT_H
#define RAX_DEUCECLIENT_CLIENT_H

#include "vault.h"
#include "file.h"

#include <httpverbs/header_dict.h>

namespace rax
{
namespace deuceclient
{

struct client
{
	explicit client(std::string host, std::string project_id);

	vault create_vault(stdex::string_view name);
	vault get_vault(stdex::string_view name);
	void delete_vault(stdex::string_view name);

	void upload_block(stdex::string_view vaultname,
	    stdex::string_view blockid, stdex::string_view data);
	void download_block(stdex::string_view vaultname,
	    stdex::string_view blockid, callback);
	void delete_block(stdex::string_view vaultname,
	    stdex::string_view blockid);

	file make_file(stdex::string_view vaultname);
	file get_file(stdex::string_view vaultname, stdex::string_view fileid);
	void download_file(stdex::string_view vaultname,
	    stdex::string_view fileid, callback);
	void delete_file(stdex::string_view vaultname,
	    stdex::string_view fileid);

private:
	std::string url_for_vault(stdex::string_view name);
	std::string url_for_block(stdex::string_view vaultname,
	    stdex::string_view blockid);
	std::string url_for_file(stdex::string_view vaultname,
	    stdex::string_view fileid);

	void do_download(std::string url, callback);
	void do_delete(std::string url);

	std::string prefix_;
	httpverbs::header_dict common_hdrs_;
};

inline
client::client(std::string host, std::string project_id) :
	prefix_(std::move(host) + "/v1.0/vaults/")
{
	common_hdrs_.add("X-Project-ID: " + std::move(project_id));
}

inline
file client::get_file(stdex::string_view vaultname, stdex::string_view fileid)
{
	return file(vaultname.to_string(), fileid.to_string(), *this);
}

inline
std::string client::url_for_vault(stdex::string_view name)
{
	auto s = prefix_;
	s.append(name.data(), name.size());

	return s;
}

inline
std::string client::url_for_block(stdex::string_view vaultname,
    stdex::string_view blockid)
{
	auto s = url_for_vault(vaultname);
	s.append("/blocks/");
	s.append(blockid.data(), blockid.size());

	return s;
}

inline
std::string client::url_for_file(stdex::string_view vaultname,
    stdex::string_view fileid)
{
	auto s = url_for_vault(vaultname);
	s.append("/files/");
	s.append(fileid.data(), fileid.size());

	return s;
}

}
}

#endif

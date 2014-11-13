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
#include "exceptions.h"

#include <httpverbs/header_dict.h>

namespace rax
{
namespace deuceclient
{

struct client
{
	explicit client(std::string host, std::string project_id);

	void authenticate_with(std::function<std::string()> f);

	vault create_vault(stdex::string_view name);
	vault get_vault(stdex::string_view name);
	void delete_vault(stdex::string_view name);

	void upload_block(stdex::string_view vaultname, sha1_digest blockid,
	    stdex::string_view data);
	void download_block(stdex::string_view vaultname, sha1_digest blockid,
	    callback);
	void delete_block(stdex::string_view vaultname, sha1_digest blockid);
	void upload_bundle(stdex::string_view vaultname, bundle& bs);

	file make_file(stdex::string_view vaultname);
	void download_file(stdex::string_view vaultname,
	    stdex::string_view fileid, callback);
	void delete_file(stdex::string_view vaultname,
	    stdex::string_view fileid);

	auto assign_blocks(stdex::string_view vaultname,
	    stdex::string_view fileid, block_arrangement& ba)
		-> std::vector<sha1_digest>;
	void finalize_file(stdex::string_view vaultname,
	    stdex::string_view fileid, int64_t len);

private:
	std::string url_for_vault(stdex::string_view name) const;
	std::string url_for_block(stdex::string_view vaultname,
	    sha1_digest blockid) const;
	std::string url_for_file(stdex::string_view vaultname,
	    stdex::string_view fileid) const;

	void do_download(std::string&& url, callback&&);
	void do_delete(std::string&& url);

	template <int Code, typename F>
	auto get_response(F do_req) -> decltype(do_req());

	std::string prefix_;
	httpverbs::header_dict common_hdrs_;
	std::function<std::string()> auth_;
};

inline
client::client(std::string host, std::string project_id) :
	prefix_(std::move(host) + "/v1.0/vaults/")
{
	common_hdrs_.add("X-Project-ID", project_id);
}

inline
void client::authenticate_with(std::function<std::string()> f)
{
	common_hdrs_.set("X-Auth-Token", f());
	auth_ = std::move(f);
}

inline
std::string client::url_for_vault(stdex::string_view name) const
{
	auto s = prefix_;
	s.append(name.data(), name.size());

	return s;
}

inline
std::string client::url_for_block(stdex::string_view vaultname,
    sha1_digest blockid) const
{
	auto s = url_for_vault(vaultname);
	auto sz = s.size();

	s.resize(sz + 8 + blockid.size() * 2);
	hashlib::detail::hexlify_to(blockid, std::copy_n("/blocks/", 8,
	    begin(s) + sz));

	return s;
}

inline
std::string client::url_for_file(stdex::string_view vaultname,
    stdex::string_view fileid) const
{
	auto s = url_for_vault(vaultname);
	auto sz = s.size();

	s.resize(sz + 7 + fileid.size());
	std::copy(begin(fileid), end(fileid), std::copy_n("/files/", 8,
	    begin(s) + sz) - 1);

	return s;
}

template <int Code, typename F>
inline
auto client::get_response(F do_req) -> decltype(do_req())
{
	auto resp = do_req();

	if (resp.status_code != Code)
		throw error(resp.status_code);

	return resp;
}

inline
void client::delete_vault(stdex::string_view name)
{
	do_delete(url_for_vault(name));
}

inline
void client::download_block(stdex::string_view vaultname, sha1_digest blockid,
    callback f)
{
	do_download(url_for_block(vaultname, blockid), std::move(f));
}

inline
void client::delete_block(stdex::string_view vaultname, sha1_digest blockid)
{
	do_delete(url_for_block(vaultname, blockid));
}

inline
void client::download_file(stdex::string_view vaultname,
    stdex::string_view fileid, callback f)
{
	do_download(url_for_file(vaultname, fileid), std::move(f));
}

inline
void client::delete_file(stdex::string_view vaultname,
    stdex::string_view fileid)
{
	do_delete(url_for_file(vaultname, fileid));
}

}
}

#endif

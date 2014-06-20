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

#ifndef RAX_DEUCECLIENT_VAULT_H
#define RAX_DEUCECLIENT_VAULT_H

#include "__common.h"
#include "clientfwd.h"
#include "file.h"

namespace rax
{
namespace deuceclient
{

struct vault
{
	explicit vault(std::string name, client& handle) :
		client_(handle), vaultname_(std::move(name))
	{}

private:
	client& client_;
	std::string vaultname_;
};

}
}

#endif

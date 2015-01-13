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

#ifndef RAX_DEUCECLIENT_BUNDLE_UPLOADER_H
#define RAX_DEUCECLIENT_BUNDLE_UPLOADER_H

#include "bundle.h"
#include "file.h"

namespace rax
{
namespace deuceclient
{

template <typename Upload>
struct file_uploader
{
	explicit file_uploader(Upload f) : upload_(std::move(f))
	{}

	explicit file_uploader(size_t max_payload, Upload f) :
		bn_(max_payload), upload_(std::move(f))
	{}

#if defined(_MSC_VER) && _MSC_VER < 1900

	file_uploader(file_uploader&& other) :
		bn_(std::move(other.bn_)),
		ba_(std::move(other.ba_)),
		upload_(std::move(other.upload_))
	{}

	file_uploader& operator=(file_uploader&& other)
	{
		bn_ = std::move(other.bn_);
		ba_ = std::move(other.ba_);
		upload_ = std::move(other.upload_);
	}

#endif

	template <typename Bundle, typename Reader>
	int64_t consume_all(file& fr, Bundle& bs, Reader&& f)
	{
		int64_t file_size = 0;
		bool bundle_is_full;

		do
		{
			bundle_is_full = bs.consume(std::forward<Reader>(f));

			if (bs.empty())
				break;

			int64_t offset = file_size;

#if !(defined(_MSC_VER) && _MSC_VER < 1800)
			for (auto&& t : bs.blocks())
#else
			for each (auto&& t in bs.blocks())
#endif
			{
				size_t end_of_block;
				sha1_digest blockid;
				std::tie(end_of_block, blockid) = t;

				ba_.add(blockid, offset);
				offset = file_size + end_of_block;
			}

			file_size += bs.size();
			feed_missing_blocks(fr.assign_blocks(ba_), bs);
			bs.clear();

		} while (bundle_is_full);

		return file_size;
	}

	template <typename Range>
	void feed_missing_blocks(Range const& ids, bundle const& bs)
	{
		auto it = begin(bs.blocks());
		auto ed = end(bs.blocks());

#if !(defined(_MSC_VER) && _MSC_VER < 1800)
		for (auto&& id : ids)
#else
		for each (auto&& id in ids)
#endif
		{
			it = std::find_if(it, ed,
			    [&](decltype(*it) binfo)
			    {
				return std::get<1>(binfo) == id;
			    });

			if (it == ed)
				break;

			if (not fits_in_block(it, bs))
				upload_(bn_);

			bs.copy_block(it, bn_);
		}
	}

	void finish()
	{
		if (not bn_.empty())
			upload_(bn_);
	}

private:
	template <typename Iter>
	bool fits_in_block(Iter it, bundle const& bs) const
	{
		return bs.serialized_size_of_block(it) <=
		    bn_.capacity() - bn_.serialized_size();
	}

	bundle bn_;
	block_arrangement ba_;
	Upload upload_;
};

template <typename Upload>
inline
auto make_file_uploader(Upload&& f)
	-> file_uploader<Upload>
{
	return file_uploader<Upload>(std::forward<Upload>(f));
}

template <typename Upload>
inline
auto make_file_uploader(size_t max_payload, Upload&& f)
	-> file_uploader<Upload>
{
	return file_uploader<Upload>(max_payload, std::forward<Upload>(f));
}

}
}

#endif

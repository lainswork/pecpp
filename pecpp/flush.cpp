#include <fstream>
#include "image.hpp"

namespace pecpp
{
	void Image::flush_raw_to_disk(std::string& filepath)
	{
		std::string backup_filepath = filepath + ".bak";
		backup(backup_filepath);

		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::ofstream out_stream(filepath, std::ios::out | std::ios::binary);
		std::ostreambuf_iterator iter(out_stream);
		std::copy(raw_.begin(), raw_.end(), iter);
	}

	void Image::backup(std::string& filepath)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::ofstream out_stream(filepath, std::ios::out | std::ios::binary);
		std::ostreambuf_iterator iter(out_stream);
		std::copy(bak_.begin(), bak_.end(), iter);
	}

	// Quite possibly the hardest thing to do correctly in the entire project
	// 
	// This function needs to basically re-link an image from a raw image, requiring
	// correctness and completeness both in parsing and in serialization.
	//
	// This function needs to ensure that:
	//	- all headers are updated
	//  - all data structures (and their order) are accounted for (including rich header)
	//  - relocations are updated if any changes that have been made require this have been made
	//  - overlays are preserved
	//  - probably other things that I will forget until this function bricks an executable
	void Image::serialize()
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::vector<uint8_t> new_raw;
		
		// get pointers to raw headers and sections.
		// these will simply be overwritten wherever possible with
		// `this` members
		auto dos = Parser::get_dos(raw_);
		auto nth = Parser::get_nth(raw_);
		auto fh = Parser::get_fh(raw_);
		auto opt = Parser::get_opt(raw_);

		// These three should never grow or shrink
		*dos = hdr_dos_;
		*nth = hdr_nt_;
		*fh = hdr_file_;
		*opt = hdr_opt_;

		// The section headers can change though (as can the sections themselves)
		auto sec_hdrs = Parser::get_shared_sec_hdrs_ptr(raw_);
		auto sec_data = Parser::get_shared_sec_data_ptr(raw_);
		size_t curr_raw_hdr_offset = *sec_hdrs.first.get() - raw_.data();
		size_t curr_raw_sec_offset = *sec_data.first.get() - raw_.data();

		for (auto sec_mapping : secs_)
		{
			auto hdr_it = raw_.begin() + curr_raw_hdr_offset;
			auto new_hdr = sec_mapping.first;
			auto new_hdr_vec = std::vector<uint8_t>((uint8_t)&new_hdr);

			auto sec_it = raw_.begin() + curr_raw_sec_offset;
			auto new_sec = sec_mapping.second;
			auto new_sec_vec = std::vector<uint8_t>((uint8_t)&new_sec);

			raw_.insert(hdr_it, new_hdr_vec.begin(), new_hdr_vec.end());
			raw_.insert(sec_it, new_sec_vec.begin(), new_sec_vec.end());

			curr_raw_hdr_offset += sizeof(image_sec_header);
			curr_raw_sec_offset += new_hdr.SizeOfRawData; // TODO Fix the padding???
		}
	}
}
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
		std::vector<uint8_t> new_raw(raw_.size());
		const auto sec_it = new_raw.begin();
		
		// get pointers to raw headers and sections.
		// these will simply be overwritten wherever possible with
		// `this` members
		auto dos_offset = Parser::get_dos_offset(raw_);
		auto nth_offset = Parser::get_nth_offset(raw_);
		auto fh_offset =  Parser::get_fh_offset(raw_);
		auto opt_offset = Parser::get_opt_offset(raw_);
		auto sec_hdrs_offset = Parser::get_sec_hdrs_offset(raw_);

		// Would be useful to have class member byte reps of the headers
		auto dos_ptr = reinterpret_cast<uint8_t*>(&hdr_dos_);
		auto nth_ptr = reinterpret_cast<uint8_t*>(&hdr_nt_);
		auto file_ptr = reinterpret_cast<uint8_t*>(&hdr_file_);
		auto opt_ptr = reinterpret_cast<uint8_t*>(&hdr_opt_);
		
		std::vector<uint8_t> new_dos(dos_ptr, dos_ptr + sizeof(hdr_dos_));
		std::vector<uint8_t> new_nth(nth_ptr, nth_ptr + sizeof(hdr_nt_));
		std::vector<uint8_t> new_file(file_ptr, file_ptr + sizeof(hdr_file_));
		std::vector<uint8_t> new_opt(opt_ptr, opt_ptr + sizeof(hdr_opt_));

		// Probably need to calculate padding for these offsets
		set_raw(dos_offset, new_dos, new_raw);
		set_raw(nth_offset, new_nth, new_raw);
		set_raw(fh_offset, new_file, new_raw);
		set_raw(opt_offset, new_opt, new_raw);	

		for (const auto hdr_sec_pair : this->secs_)
		{
			auto hdr = hdr_sec_pair.first;
			auto hdr_ptr = reinterpret_cast<uint8_t*>(&hdr);
			auto sec = hdr_sec_pair.second;


			std::vector<uint8_t> new_sec_hdr(hdr_ptr, hdr_ptr + sizeof(image_sec_header));

			auto sec_data_offset = hdr.PointerToRawData;
			set_raw(sec_hdrs_offset, new_sec_hdr, new_raw);
			set_raw(sec_data_offset, sec, new_raw);
			sec_hdrs_offset += sizeof(image_sec_header);
		}

		this->raw_ = new_raw;
		refresh(raw_);


		//*dos = hdr_dos_;
		//*nth = hdr_nt_;
		//*fh = hdr_file_;
		//*opt = hdr_opt_;

		//// The section headers can change though (as can the sections themselves)
		//auto sec_hdrs = Parser::get_shared_sec_hdrs_ptr(raw_);
		//auto sec_data = Parser::get_shared_sec_data_ptr(raw_);

		//size_t curr_raw_hdr_offset = *sec_hdrs.first.get() - raw_.data();

		//for (auto sec_mapping : secs_)
		//{
		//	auto new_hdr = sec_mapping.first;
		//	auto const new_hdr_ptr = reinterpret_cast<uint8_t*>(&new_hdr);
		//	std::vector<uint8_t> new_hdr_vec(new_hdr_ptr, new_hdr_ptr + sizeof(new_hdr));

		//	auto new_sec_vec = sec_mapping.second;
		//	size_t curr_raw_sec_offset = new_hdr.PointerToRawData;

		//	auto sec_it = raw_.begin() + curr_raw_hdr_offset; // need to check header as well
		//	auto sec_end = raw_.begin() + raw_.size();
		//	if (sec_it > sec_end)
		//	{
		//		raw_.resize(raw_.capacity() + new_sec_vec.size());
		//	}

		//	raw_.insert(raw_.begin() + curr_raw_hdr_offset, new_hdr_vec.begin(), new_hdr_vec.end());
		//	raw_.insert(raw_.begin() + curr_raw_sec_offset, new_sec_vec.begin(), new_sec_vec.end()); // something is wrong here			

		//	curr_raw_hdr_offset += sizeof(image_sec_header);
		//}
	}

	//void Image::serialize2()
	//{
	//	const std::lock_guard<std::mutex> lock(this_mtx_);
	//	auto dos = Parser::get_dos(raw_);
	//	auto nth = Parser::get_nth(raw_);
	//	auto fh = Parser::get_fh(raw_);
	//	auto opt = Parser::get_opt(raw_);

	//	// The section headers can change though (as can the sections themselves)
	//	auto sec_hdrs = Parser::get_shared_sec_hdrs_ptr(raw_);
	//	auto sec_data = Parser::get_shared_sec_data_ptr(raw_);
	//	size_t curr_raw_hdr_offset = *sec_hdrs.first.get() - raw_.data();
	//	size_t curr_raw_sec_offset = *sec_data.first.get() - raw_.data();
	//}
}
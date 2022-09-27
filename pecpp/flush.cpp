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

	void Image::serialize()
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::vector<uint8_t> new_raw;
		
		auto dos = Parser::get_dos(raw_);
		auto nth = Parser::get_nth(raw_);
		auto fh = Parser::get_fh(raw_);
		auto opt = Parser::get_opt(raw_);
		auto sec_hdrs = Parser::get_shared_sec_hdrs_ptr(raw_);



	}
}
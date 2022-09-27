#include <fstream>
#include "image.hpp"

namespace pecpp
{
	void Image::flush(std::string& filepath)
	{
		std::string backup_filepath = filepath + ".bak";
		backup(backup_filepath);

		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::ofstream out_stream(filepath, std::ios::out | std::ios::binary);
		out_stream.write((const char*)raw_.data(), raw_.size());
	}

	void Image::backup(std::string& filepath)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		std::ofstream out_stream(filepath, std::ios::out | std::ios::binary);
		out_stream.write((const char*)bak_.data(), bak_.size());
	}
}
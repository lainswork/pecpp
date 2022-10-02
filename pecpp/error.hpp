#pragma once

#include <stdexcept>

namespace pecpp
{
	struct Error
	{
		inline static std::runtime_error err_dos_sig_invalid =
			std::runtime_error("DOS signature invalid");
		inline static std::runtime_error err_nt_sig_invalid =
			std::runtime_error("NT signature invalid");
		inline static std::runtime_error err_opt_magic_invalid
			= std::runtime_error("Optional header magic invalid");
		inline static std::runtime_error err_sec_size_mismatch
			= std::runtime_error("Section header size / actual data size mismatch");
		inline static std::runtime_error err_sec_not_found
			= std::runtime_error("Section not found");
		inline static std::invalid_argument err_invalid_memory
			= std::invalid_argument("Argument was nullptr");
		inline static std::out_of_range err_raw_assignment 
			= std::out_of_range("Assignment to raw PE data out of range");
		inline static std::out_of_range err_sec_out_of_range
			= std::out_of_range("Section data out of range of image data vector");
		inline static std::out_of_range err_data_out_of_range_assignment
			= std::out_of_range("Data assignment out of range");
	};
}
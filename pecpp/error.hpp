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
		inline static std::invalid_argument err_raw_assignment =
			std::invalid_argument("Assignment to raw PE data out of range");
	};
}
#pragma once

#include "pecpp.hpp"

namespace pecpp
{
	struct Parser
	{
		static image_dos_header* get_dos(std::vector<uint8_t>& data);
		static image_nt_headers* get_nth(std::vector<uint8_t>& data);
		static image_opt_header* get_opt(std::vector<uint8_t>& data);
		static image_file_header* get_fh(std::vector<uint8_t>& data);
		static std::vector<image_sec_header> get_sec_hdrs(std::vector<uint8_t>& data);
		static std::vector<std::vector<uint8_t>>  get_secs(std::vector<uint8_t>& data);
		static sec_map get_sec_map(std::vector<uint8_t>& data);

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
		};
	};
}
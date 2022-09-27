#pragma once

#include "pecpp.hpp"

namespace pecpp
{
	struct Parser
	{
		static image_dos_header* get_dos(std::vector<uint8_t>& image);
		static image_nt_headers* get_nth(std::vector<uint8_t>& image);
		static image_opt_header* get_opt(std::vector<uint8_t>& image);
		static image_file_header* get_fh(std::vector<uint8_t>& image);
		static image_sec_header* get_sec_hdr(std::string& name, std::vector<uint8_t>& image);
		static std::vector<image_sec_header> get_sec_hdrs(std::vector<uint8_t>& image);
		static std::vector<std::vector<uint8_t>>  get_secs(std::vector<uint8_t>& image);
		static sec_map get_sec_map(std::vector<uint8_t>& image);

		static void set_dos(image_dos_header* new_dos, std::vector<uint8_t>& image);
		static void set_nth(image_nt_headers* new_nth, std::vector<uint8_t>& image);
		static void set_opt(image_opt_header* new_opt, std::vector<uint8_t>& image);
		static void set_fh(image_file_header* new_fh, std::vector<uint8_t>& image);
		static void set_sec_hdr(std::string& sec_name, image_sec_header* new_sec_hdr, std::vector<uint8_t>& image);
	};
}
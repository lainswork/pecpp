#pragma once

#include "pecpp.hpp"
#include "parser.hpp"

namespace pecpp
{
	class Image
	{
	public:

		Image(std::vector<uint8_t>& vec)
			: raw_{ vec }
			, bak_{ vec }
			, hdr_dos_{ *Parser::get_dos(vec) }
			, hdr_nt_{ *Parser::get_nth(vec) }
			, hdr_file_{ *Parser::get_fh(vec) }
			, hdr_opt_{ *Parser::get_opt(vec) }
			, secs_{ Parser::get_sec_map(vec) }
		{};

		Image& operator=(Image& other);

		image_dos_header get_dos() const;
		image_nt_headers get_nth() const;
		image_opt_header get_opt() const;
		image_file_header get_fh() const;
		image_sec_header get_sec_hdr(std::string& name) const;
		std::vector<uint8_t> get_sec_data(std::string& name) const;
		sec_map get_sec_map() const;

		void set_sec_data(std::string& sec_name, std::vector<uint8_t> new_data);
		void set_sec_hdr(std::string& sec_name, image_sec_header* new_hdr);

	private:
		image_dos_header hdr_dos_;
		image_nt_headers hdr_nt_;
		image_opt_header hdr_opt_;
		image_file_header hdr_file_;
		sec_map secs_;

		std::mutex raw_mtx_;
		std::mutex bak_mtx_;
		std::vector<uint8_t> raw_;
		std::vector<uint8_t> bak_;

		void refresh(std::vector<uint8_t>& new_raw);
		void set_raw(uint32_t offset, std::vector<uint8_t> data);

	};
}
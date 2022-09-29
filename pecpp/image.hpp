#pragma once

#include "pecpp.hpp"
#include "parser.hpp"

namespace pecpp
{
	class Image
	{
	public:
		explicit Image(std::vector<uint8_t>& vec)
			: raw_{ vec }
			, bak_{ vec }
			, hdr_dos_{ *Parser::get_dos(vec) }
			, hdr_nt_{ *Parser::get_nth(vec) }
			, hdr_file_{ *Parser::get_fh(vec) }
			, hdr_opt_{ *Parser::get_opt(vec) }
			, secs_{ Parser::get_sec_map(vec) }
		{};

		Image& operator=(Image& other);

		Image(const Image&& other)
			: raw_{ std::move(other.raw_) }
			, bak_{ std::move(other.bak_) }
			, hdr_dos_{ std::move(other.hdr_dos_) }
			, hdr_nt_{ std::move(other.hdr_nt_) }
			, hdr_file_{ std::move(other.hdr_file_) }
			, hdr_opt_{ std::move(other.hdr_opt_) }
			, secs_{ std::move(other.secs_) }
			{};

		// getters
		image_dos_header get_dos() const;
		image_nt_headers get_nth() const;
		image_opt_header get_opt() const;
		image_file_header get_fh() const;
		sec_map get_sec_map() const;
		image_sec_header get_sec_hdr(std::string& name) const;
		std::vector<uint8_t> get_sec_data(std::string& name) const;

		// setters
		void set_sec_data(std::string& sec_name, std::vector<uint8_t> new_data);
		void new_sec(std::string& name, uint32_t characteristics, std::vector<uint8_t>& new_data);

		// file manipulation / flushing raw to Image
		void flush_raw_to_disk(std::string& filepath);
		void serialize(); // regenerate raw_ based on Image members (oh god)


	private:
		image_dos_header hdr_dos_;
		std::vector<uint8_t> dos_stub_;
		
		image_nt_headers hdr_nt_;
		image_opt_header hdr_opt_;
		image_file_header hdr_file_;

		sec_map secs_;

		mutable std::mutex this_mtx_;
		std::vector<uint8_t> raw_;
		const std::vector<uint8_t> bak_;

		void refresh(std::vector<uint8_t>& new_raw);
		void set_raw(uint32_t offset, std::vector<uint8_t> data);
		void set_raw(uint32_t offset, std::vector<uint8_t>& src, std::vector<uint8_t>& dst);

		void backup(std::string& filepath);
	};
}
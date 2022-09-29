#include "parser.hpp"

namespace pecpp
{
	//
	// Getters
	//
	image_dos_header* Parser::get_dos(std::vector<uint8_t>& image)
	{
		auto dos = reinterpret_cast<image_dos_header*>(image.data());
		if (dos->e_magic != IMAGE_DOS_SIGNATURE) throw Error::err_dos_sig_invalid;
		return dos;
	}

	image_nt_headers* Parser::get_nth(std::vector<uint8_t>& image)
	{
		auto dos = get_dos(image);
		auto nth = reinterpret_cast<image_nt_headers*>(image.data() + dos->e_lfanew);
		if (nth->Signature != IMAGE_NT_SIGNATURE) throw Error::err_nt_sig_invalid;
		return nth;
	}

	image_opt_header* Parser::get_opt(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		auto opt = reinterpret_cast<image_opt_header*>(&nth->OptionalHeader);
		// whats the worst that can happen? lol
		return opt;
	}

	image_file_header* Parser::get_fh(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		auto file = reinterpret_cast<image_file_header*>(&nth->FileHeader);
		return file;
	}

	image_sec_header* Parser::get_sec_hdr(std::string& name, std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		std::vector<image_sec_header> sec_hdrs;
		auto sec = IMAGE_FIRST_SECTION(nth);
		uint32_t i = 0;

		while (i <= nth->FileHeader.NumberOfSections)
		{
			if (!name.compare((char*)sec->Name))
			{
				return sec;
			}
			sec = IMAGE_NEXT_SECTION(sec);
			i++;
		}

		throw Error::err_sec_not_found;
	}

	std::vector<image_sec_header> Parser::get_sec_hdrs(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		std::vector<image_sec_header> sec_hdrs;
		auto sec = IMAGE_FIRST_SECTION(nth);
		uint32_t i = 0;

		while (i < nth->FileHeader.NumberOfSections)
		{
			sec_hdrs.push_back(*sec);
			sec = IMAGE_NEXT_SECTION(sec);
			i++;
		}

		return sec_hdrs;
	}

	std::vector<std::vector<uint8_t>> Parser::get_secs(std::vector<uint8_t>& data)
	{
		auto sec_hdrs = get_sec_hdrs(data);
		std::vector<std::vector<uint8_t>> secs;
		for (auto sec_hdr : sec_hdrs)
		{
			if (sec_hdr.PointerToRawData + sec_hdr.SizeOfRawData > data.size())
				throw Error::err_sec_out_of_range;
			uint8_t* ptr = data.data() + sec_hdr.PointerToRawData;

			std::vector<uint8_t> sec(ptr, ptr + sec_hdr.SizeOfRawData);
			secs.push_back(sec);
		}

		return secs;
	}

	std::vector<uint8_t> Parser::get_overlay(std::vector<uint8_t>& image)
	{
		auto sec_hdrs = get_sec_hdrs(image);
		auto last_sec_end_offset = sec_hdrs[-1].PointerToRawData + sec_hdrs[-1].SizeOfRawData;
		std::vector<uint8_t> overlay(image.begin() + last_sec_end_offset, image.end()); // TODO Make this safer
	}

	sec_map Parser::get_sec_map(std::vector<uint8_t>& data)
	{
		auto sec_hdrs = get_sec_hdrs(data);
		auto sec_data = get_secs(data);
		sec_map map;

		if (sec_hdrs.size() != sec_data.size()) throw Error::err_sec_size_mismatch;

		for (auto it = sec_hdrs.begin(); it != sec_hdrs.end(); it++)
		{
			int idx = std::distance(sec_hdrs.begin(), it);
			auto hdr = sec_hdrs[idx];
			auto raw = sec_data[idx];
			auto pair = std::make_pair(hdr, raw);
			map.push_back(pair);
		}

		return map;
	}

	bool Parser::overlay_present(std::vector<uint8_t>& image)
	{
		auto sec_hdrs = get_sec_hdrs(image);
		auto last_sec_end_offset = sec_hdrs[-1].PointerToRawData + sec_hdrs[-1].SizeOfRawData;
		if (last_sec_end_offset <= image.size()) return false; // TODO Sanity check this
		return true;
	}

	size_t Parser::get_sec_hdrs_size(std::vector<uint8_t>& image)
	{
		auto fh = get_fh(image);
		return fh->NumberOfSections * sizeof(image_sec_header);
	}

	region_ptr Parser::get_shared_sec_hdrs_ptr(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		uint8_t* sec = (uint8_t*)IMAGE_FIRST_SECTION(nth);
		auto ptr = std::make_shared<uint8_t*>(sec);
		auto size = get_sec_hdrs_size(image);
		auto pair = std::make_pair(ptr, size);
		return pair;
	}

	region_ptr Parser::get_shared_sec_data_ptr(std::vector<uint8_t>& image)
	{
		auto sec_hdrs = get_sec_hdrs(image);
		auto first_section_offset = sec_hdrs[0].PointerToRawData;
		size_t total_section_data_size = 0;

		for (auto hdr : sec_hdrs)
		{
			total_section_data_size = total_section_data_size + hdr.SizeOfRawData;
		}

		if ((first_section_offset + total_section_data_size) > image.size())
			throw Error::err_sec_out_of_range;

		auto sec_data_ptr = sec_hdrs[0].PointerToRawData + image.data();
		auto ptr = std::make_shared<uint8_t*>(sec_data_ptr);
		auto pair = std::make_pair(ptr, total_section_data_size);
		return pair;
	}

	size_t Parser::get_dos_offset(std::vector<uint8_t>& image)
	{
		auto dos = get_dos(image);
		return reinterpret_cast<uint8_t*>(dos) - image.data();
	}

	size_t Parser::get_nth_offset(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		return reinterpret_cast<uint8_t*>(nth) - image.data();
	}

	size_t Parser::get_fh_offset(std::vector<uint8_t>& image)
	{
		auto fh = get_fh(image);
		return reinterpret_cast<uint8_t*>(fh) - image.data();
	}

	size_t Parser::get_opt_offset(std::vector<uint8_t>& image)
	{
		auto opt = get_opt(image);
		return reinterpret_cast<uint8_t*>(opt) - image.data();
	}

	size_t Parser::get_sec_hdrs_offset(std::vector<uint8_t>& image)
	{
		auto nth = get_nth(image);
		auto sec = IMAGE_FIRST_SECTION(nth);
		return reinterpret_cast<uint8_t*>(sec) - image.data();
	}


	//
	// Setters
	//
	void Parser::set_dos(image_dos_header* new_dos, std::vector<uint8_t>& image)
	{
		if (new_dos == nullptr) throw Error::err_invalid_memory;
		auto dos = get_dos(image);
		*dos = *new_dos;
	}

	void Parser::set_nth(image_nt_headers* new_nth, std::vector<uint8_t>& image)
	{
		if (new_nth == nullptr) throw Error::err_invalid_memory;
		auto nth = get_nth(image);
		*nth = *new_nth;
	}

	void Parser::set_opt(image_opt_header* new_opt, std::vector<uint8_t>& image)
	{
		if (new_opt == nullptr) throw Error::err_invalid_memory;
		auto opt = get_opt(image);
		*opt = *new_opt;
	}

	void Parser::set_fh(image_file_header* new_fh, std::vector<uint8_t>& image)
	{
		if (new_fh == nullptr) throw Error::err_invalid_memory;
		auto fh = get_fh(image);
		*fh = *new_fh;
	}

	void Parser::set_sec_hdr(std::string& sec_name, image_sec_header* new_sec_hdr, std::vector<uint8_t>& image)
	{
		if (new_sec_hdr == nullptr) throw Error::err_invalid_memory;
		auto hdr = get_sec_hdr(sec_name, image);
		*hdr = *new_sec_hdr;
	}
}
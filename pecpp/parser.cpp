#include "parser.hpp"

namespace pecpp
{
	image_dos_header* Parser::get_dos(std::vector<uint8_t>& data)
	{
		auto dos = reinterpret_cast<image_dos_header*>(data.data());
		if (dos->e_magic != IMAGE_DOS_SIGNATURE) throw Error::err_dos_sig_invalid;
		return dos;
	}

	image_nt_headers* Parser::get_nth(std::vector<uint8_t>& data)
	{
		auto dos = get_dos(data);
		auto nth = reinterpret_cast<image_nt_headers*>(data.data() + dos->e_lfanew);
		if (nth->Signature != IMAGE_NT_SIGNATURE) throw Error::err_nt_sig_invalid;
		return nth;
	}

	image_opt_header* Parser::get_opt(std::vector<uint8_t>& data)
	{
		auto nth = get_nth(data);
		auto opt = reinterpret_cast<image_opt_header*>(&nth->OptionalHeader);
		if (opt->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) throw Error::err_opt_magic_invalid;
		return opt;
	}

	image_file_header* Parser::get_fh(std::vector<uint8_t>& data)
	{
		auto nth = get_nth(data);
		auto file = reinterpret_cast<image_file_header*>(&nth->FileHeader);
		return file;
	}

	std::vector<image_sec_header> Parser::get_sec_hdrs(std::vector<uint8_t>& data)
	{
		auto nth = get_nth(data);
		std::vector<image_sec_header> sec_hdrs;
		auto sec = IMAGE_FIRST_SECTION(nth);
		uint32_t i = 0;

		while (i <= nth->FileHeader.NumberOfSections)
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
			uint8_t* ptr = data.data() + sec_hdr.SizeOfRawData;
			std::vector<uint8_t> sec(ptr, ptr + sec_hdr.SizeOfRawData);
			secs.push_back(sec);
		}

		return secs;
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
}
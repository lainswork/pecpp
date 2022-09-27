// pecpp.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <map>
#include <algorithm>
#include <memory>
#include <cmath>

#include "error.hpp"

#define IMAGE_NEXT_SECTION( firstsection ) ((PIMAGE_SECTION_HEADER) \
    ((ULONG_PTR)(firstsection) + sizeof(IMAGE_SECTION_HEADER)))

namespace pecpp
{
	// Redefinition of Windows types to keep things looking tidy
	typedef IMAGE_DOS_HEADER image_dos_header;
	typedef IMAGE_NT_HEADERS image_nt_headers;
	typedef IMAGE_OPTIONAL_HEADER image_opt_header;
	typedef IMAGE_FILE_HEADER image_file_header;
	typedef IMAGE_SECTION_HEADER image_sec_header;

	// pecpp special types
	typedef std::pair<image_sec_header, std::vector<uint8_t>> sec_pair;
	typedef std::vector<sec_pair> sec_map;
	typedef std::pair<std::shared_ptr<uint8_t*>, size_t> region_ptr;
}
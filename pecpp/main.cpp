#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include "pecpp.hpp"
#include "image.hpp"

namespace helpers
{
	std::vector<uint8_t> file_to_bytes(std::string const& filename)
	{
		// open the file:
		std::ifstream file(filename, std::ios::binary);

		// Stop eating new lines in binary mode!!!
		file.unsetf(std::ios::skipws);

		// get its size:
		std::streampos fileSize;

		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// reserve capacity
		std::vector<uint8_t> vec;
		vec.reserve(fileSize);

		// read the data:
		vec.insert(vec.begin(),
			std::istream_iterator<BYTE>(file),
			std::istream_iterator<BYTE>());

		return vec;
	}
}

namespace test
{
	class Tests
	{
	public:
		static void test_image_constr_speed()
		{
			namespace fs = std::filesystem;
			const fs::path sys32{ fs::path("C:/Windows/System32") };
			std::vector<pecpp::Image> images;
			for (auto entry : fs::directory_iterator(sys32))
			{
				if (entry.is_regular_file())
				{
					auto extension = entry.path().extension().string();
					if (extension.compare(".exe") != 0) continue;
					std::string file_name = entry.path().string();
					auto data = helpers::file_to_bytes(file_name);
					try
					{
						pecpp::Image image(data);
						images.push_back(std::move(image));
					}
					catch (...)
					{
						continue;
					}
				}
			}
		}
	};
}

int main()
{	
	test::Tests::test_image_constr_speed();
}

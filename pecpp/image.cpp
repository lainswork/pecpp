#include "image.hpp"

namespace pecpp
{
	//
	// Operators
	//
	Image& Image::operator=(Image& other)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		hdr_dos_ = other.get_dos();
		hdr_nt_ = other.get_nth();
		hdr_file_ = other.get_fh();
		hdr_opt_ = other.get_opt();
		secs_ = other.get_sec_map();
		raw_ = other.raw_;
		return *this;
	}

	//
	// Getters
	//

	image_dos_header Image::get_dos() const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		return this->hdr_dos_;
	}

	image_nt_headers Image::get_nth() const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		return this->hdr_nt_;
	}

	image_opt_header Image::get_opt() const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		return this->hdr_opt_;
	}

	image_file_header Image::get_fh() const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		return this->hdr_file_;
	}

	image_sec_header Image::get_sec_hdr(std::string& name) const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);

		for (auto sec : this->secs_)
		{
			auto hdr = sec.first;
			if (!name.compare((char*)hdr.Name))
				return hdr;
		}
	}

	std::vector<uint8_t> Image::get_sec_data(std::string& name) const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);

		for (auto sec : this->secs_)
		{
			auto hdr = sec.first;
			if (!name.compare((char*)hdr.Name))
				return sec.second;
		}
	}

	sec_map Image::get_sec_map() const
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);

		return this->secs_;
	}

	//
	// Setters
	//

	// TODO Debug this to see if its actually working
	void Image::set_sec_data(std::string& sec_name, std::vector<uint8_t> new_data)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);

		for (auto sec : this->secs_)
		{
			auto hdr = sec.first;
			if (!sec_name.compare((char*)hdr.Name))
			{
				auto sec_data = sec.second;
				// TODO resize raw at destination to allow for data
				// requires fixup of size variables in section header, optional header
				set_raw(hdr.PointerToRawData, new_data);
			}
		}
	}

	void Image::set_sec_hdr(std::string& sec_name, image_sec_header* new_hdr)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		if (new_hdr == nullptr) throw Error::err_invalid_memory;
		Parser::set_sec_hdr(sec_name, new_hdr, raw_);		
		refresh(raw_);
	}

	void Image::new_sec(image_sec_header* new_hdr, std::vector<uint8_t>& new_data)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);


	}

	//
	// Private image utilities
	//

	void Image::set_raw(uint32_t offset, std::vector<uint8_t> data)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		// TODO remove size limitations by dynamically expanding raw
		if (raw_.size() < data.size() + offset)
			throw Error::err_raw_assignment;

		for (auto byte : data)
		{
			raw_[offset] = byte;
		}

		refresh(raw_); // Does this work???
	}

	void Image::refresh(std::vector<uint8_t>& new_raw)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		auto new_img = Image(new_raw);
		*this = new_img;
	}
}
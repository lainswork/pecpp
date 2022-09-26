#include "image.hpp"

namespace pecpp
{
	Image& Image::operator=(Image& other)
	{
		const std::lock_guard<std::mutex> lock_raw(raw_mtx_);
		const std::lock_guard<std::mutex> lock_bak(bak_mtx_);
		hdr_dos_ = other.get_dos();
		hdr_nt_ = other.get_nth();
		hdr_file_ = other.get_fh();
		hdr_opt_ = other.get_opt();
		secs_ = other.get_sec_map();
		raw_ = other.raw_;
		bak_ = this->bak_;
		return *this;
	}

	image_dos_header Image::get_dos() const
	{
		return this->hdr_dos_;
	}

	image_nt_headers Image::get_nth() const
	{
		return this->hdr_nt_;
	}

	image_opt_header Image::get_opt() const
	{
		return this->hdr_opt_;
	}

	image_file_header Image::get_fh() const
	{
		return this->hdr_file_;
	}

	image_sec_header Image::get_sec_hdr(std::string& name) const
	{
		for (auto sec : this->secs_)
		{
			auto hdr = sec.first;
			if (!name.compare((char*)hdr.Name))
				return hdr;
		}
	}

	std::vector<uint8_t> Image::get_sec_data(std::string& name) const
	{
		for (auto sec : this->secs_)
		{
			auto hdr = sec.first;
			if (!name.compare((char*)hdr.Name))
				return sec.second;
		}
	}

	sec_map Image::get_sec_map() const
	{
		return this->secs_;
	}

	// TODO Debug this to see if its actually working
	void Image::set_sec_data(std::string& sec_name, std::vector<uint8_t> new_data)
	{
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
		auto new_img = Image(raw_);
		*this = new_img;
	}

	void Image::set_raw(uint32_t offset, std::vector<uint8_t> data)
	{
		// TODO remove size limitations by dynamically expanding raw
		if (raw_.size() < data.size() + offset)
			throw Error::err_raw_assignment;

		const std::lock_guard<std::mutex> lock(raw_mtx_);
		for (auto byte : data)
		{
			raw_[offset] = byte;
		}
	}
}
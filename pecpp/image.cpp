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

	void Image::set_sec_data(std::string& sec_name, std::vector<uint8_t> new_data)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);
		for (auto pair : secs_)
		{
			if (!sec_name.compare((char*)pair.first.Name))
			{
				pair.second = new_data;
			}
		}
	}

	// TODO test this
	void Image::new_sec(std::string& name, uint32_t characteristics, std::vector<uint8_t>& new_data)
	{
		const std::lock_guard<std::mutex> lock(this_mtx_);

		image_sec_header new_hdr = { 0 };

		// Ultimately, we want some degree of indifference on the part of the serializer/image
		// to the order of these sections, so that sections can me modified/inserted wherever and not break
		// other portions of the executable. The below is and example of code that will have to change
		auto new_section = std::make_pair(new_hdr, new_data);
		auto va_last_sec_end = secs_[-1].first.VirtualAddress + secs_[-1].first.Misc.VirtualSize;
		auto factor = va_last_sec_end / hdr_opt_.SectionAlignment;
		auto new_va = std::ceil(factor) * hdr_opt_.SectionAlignment;

		std::copy(name.begin(), name.end(), new_hdr.Name);
		new_hdr.VirtualAddress = new_va;
		new_hdr.SizeOfRawData = new_data.size();
		new_hdr.Characteristics = characteristics;	

		secs_.push_back(new_section);

		hdr_file_.NumberOfSections++;
		factor = new_data.size() / hdr_opt_.SectionAlignment;
		auto size_added = std::ceil(factor) * hdr_opt_.SectionAlignment;
		hdr_opt_.SizeOfImage + size_added;
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
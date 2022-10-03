# pecpp

C++ PE/COFF parsing and manipulation library 

## Inspiration / Prior Work

This project developed out of my own need for a tool handy in both PE/COFF parsing and manipulation (e.g. adding sections, modifying headers, etc.)

There are several open source attempts at comprehensive PE/COFF parsing libraries, but to my knowledge none of them are modern C++ equipped with PE/COFF manipulation utillities. Some of these are:

- [libpe](https://github.com/merces/libpe/)
- [peparser](https://github.com/smarttechnologies/peparser)
- [bearparser](https://github.com/hasherezade/bearparser)
- [penet](https://github.com/secana/PeNet/tree/master/src/PeNet/HeaderParser)

The goal for this project is to leverage the best parts of these other projects along with modern C++ 20 to create a new parsing and manipulation library. Both of those components are things I'm learning along the way, so contributions, code reviews, and feature requests are very welcome.

## Contributing

Please see the [Contributing](https://github.com/joshfinley/pecpp/blob/main/contributing.md) guide.

## Examples

### Parsing all DLLs in System32

Pointing the parser / image wrapper at `C:\Windows\System32` makes for a decent pseudo-fuzz test:

```C++
for (auto entry : fs::directory_iterator(sys32))
{
  // get path, check extension, get vector of file bytes (omitted)
  try
  {
    pecpp::Image image(data);
    images.push_back(std::move(image));
  }
  // ...
}
```

On my machine, over 3000 `.dll`s are successfully parsed with no exceptions thrown.

### Deserialization, modification and reserialization of a DLL

```C++
// Load file from disk into a byte vector
auto data = helpers::file_to_bytes(file_name);

// Create a new Image instance from the data
pecpp::Image image(data);

// Modify the image by adding a section
image.new_sec(name, characteristics, new_data);

// Regenerate the raw byte vector representation of the image
image.serialize();

// Write to disk
image.save();
```

## Building

Please refer to the [build guide](https://github.com/joshfinley/pecpp/blob/main/building.md)
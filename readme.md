# pecpp

C++ PE/COFF parsing and manipulation library 

## Inspiration / Prior Work

This project developed out of my own need for a tool handy in both PE/COFF parsing and manipulation (e.g. adding sections, modifying headers, etc.)

There are several open source attempts at comprehensive PE/COFF parsing libraries, but to my knowledge none of them are modern C++ or equipped with PE/COFF manipulation utillities. Some of these are:

- [libpe](https://github.com/merces/libpe/)
- [peparser](https://github.com/smarttechnologies/peparser)
- [bearparser](https://github.com/hasherezade/bearparser)

The goal for this project is to leverage the best parts of these other projects along with modern C++ 20 to create a new parsing and manipulation library. Both of those components are things I'm learning along the way, so contributions, code reviews, and feature requests are very welcome.

## Metrics

Pointing the parser / image wrapper at `C:\Windows\System32` makes for a decent pseudo-fuzz test:

```
for (auto entry : fs::directory_iterator(sys32))
{
  // get path, check extension, get vector of file bytes (omitted)
  try
  {
    pecpp::Image image(data);
    images.push_back(std::move(image));
  }
  catch (std::exception& e)
  {
    continue;
  }
}
```

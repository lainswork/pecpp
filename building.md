## MSVC

The recommended way to build this project under MSVC is using Visual Studio 2022's CMake support (`File->Open->CMake`)

## Linux Mingw

If building for Linux targets, please make sure that you are using the POSIX threading model for Mingw. E.g. under Debian systems:

```
sudo update-alternatives --config x86_64-w64-mingw32-g++
There are 2 choices for the alternative x86_64-w64-mingw32-g++ (providing /usr/bin/x86_64-w64-mingw32-g++).

  Selection    Path                                   Priority   Status
------------------------------------------------------------
* 0            /usr/bin/x86_64-w64-mingw32-g++-win32   60        auto mode
  1            /usr/bin/x86_64-w64-mingw32-g++-posix   30        manual mode
  2            /usr/bin/x86_64-w64-mingw32-g++-win32   60        manual mode
# select 1
```

With this selected, you can simply run `cmake .` from the `pecpp` subdirectory to build.

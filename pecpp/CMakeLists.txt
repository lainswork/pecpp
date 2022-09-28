# CMakeList.txt : CMake project for pecpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("pecpp")

find_path(DTL_INCLUDE_DIRS "dtl/Diff.hpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

# Add source to this project's executable.
add_library (lib-pecpp SHARED "pecpp.cpp" "pecpp.hpp" "parser.hpp" "parser.cpp" "image.hpp" "image.cpp" "error.hpp" "flush.cpp")
target_include_directories(lib-pecpp PRIVATE ${DTL_INCLUDE_DIRS})

set(SOURCE_FILES pecpp.cpp)
add_executable(main ${SOURCE_FILES} "main.cpp" "parser.hpp" "parser.cpp" "image.hpp" "image.cpp" "error.hpp" "flush.cpp")
target_link_libraries(main lib-pecpp)

# Add tests
add_executable(cpp_test main.cpp "main.cpp" "parser.hpp" "parser.cpp" "image.hpp" "image.cpp" "error.hpp" "flush.cpp")
target_link_libraries(cpp_test lib-pecpp)

enable_testing()

add_test(
	NAME cpp_test
	COMMAND $<TARGET_FILE:cpp_test>
)

# TODO: Add install targets if needed.

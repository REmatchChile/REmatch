#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

// Loads a text file. Returns the contents of the file as a string.
std::string file2str(std::string filename);

// Format a size in bytes to readable-form in units of Kib, Mib, Gib
std::string formatMem(size_t sizeInBytes);

#endif // UTIL_HPP

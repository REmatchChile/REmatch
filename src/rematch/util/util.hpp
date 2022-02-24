#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <istream>
#include <fstream>
#include <vector>

namespace rematch {
namespace util {

std::string file2str(std::string filename);

struct VectorHasher {
	size_t operator()(const std::vector<size_t> &V) const {
		size_t hash = V.size();
		for(auto &i : V) {
			hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};

} // end namespace util
} // end namespace rematch

#endif // UTIL_HPP

#include "util.hpp"

namespace rematch {
namespace util {


std::string file2str(std::string filename) {
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, in.end);
    contents.resize(in.tellg());
    in.seekg(0, in.beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  throw std::runtime_error("Error loading file");
}

} // end namespace util
} // end namespace rematch
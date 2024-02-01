#include <fstream>
#include <string>
#include "evaluation/start_end_chars.hpp"

namespace rematch {

std::string add_start_and_end_chars(std::string_view document_view);
std::string read_file_and_add_start_and_end_chars(std::ifstream& rematch_file);
std::string read_file(std::ifstream& file);

}  // namespace rematch

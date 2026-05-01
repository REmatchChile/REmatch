
#include <cli11.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "REmatch/REmatch.hpp"

using namespace REmatch;

std::string read_from_file(const std::string& filename) {
  std::ifstream t(filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// TODO: File input
// TODO: Const methods if possible
int main(int argc, char** argv) {
  std::string pattern;
  std::string document;

  uint_fast32_t max_mempool_duplications{DEFAULT_MAX_MEMPOOL_DUPLICATIONS};
  uint_fast32_t max_deterministic_states{DEFAULT_MAX_DETERMINISTIC_STATES};
  uint64_t buffer_size{DEFAULT_STREAM_BUFFER_SIZE};

  bool multi_spanners{false};
  bool line_by_line{false};
  bool stream{false};
  bool findone{false};
  bool check{false};

  CLI::App app{"REmatch CLI"};
  app.get_formatter()->column_width(35);
  app.option_defaults()->always_capture_default();

  app.add_option("pattern", pattern)
      ->description("The REQL pattern to match")
      ->type_name("<pattern>")
      ->required();

  app.add_option("document", document)
      ->description("The document to match")
      ->type_name("<document>")
      ->required();

  app.add_option("--max-mempool-duplications", max_mempool_duplications)
      ->description("Maximum number of memory pool duplications allowed")
      ->type_name("<max-duplications>");

  app.add_option("--max-deterministic-states", max_deterministic_states)
      ->description("Maximum number of deterministic states allowed")
      ->type_name("<max-states>");

  app.add_option("--stream-buffer-size", buffer_size)
      ->description("Size (bytes) for the buffer used in streams")
      ->type_name("<buffer-size>");

  app.add_flag("-m,--multi-spanners", multi_spanners)->description("Enable Multi Spanners support");

  app.add_flag("-l,--line-by-line", line_by_line)->description("Enable line by line");

  app.add_flag("-s,--stream", stream);

  app.add_flag("-o,--findone", findone);

  app.add_flag("-c", check);

  CLI11_PARSE(app, argc, argv);

  Flags flags = Flags::NONE;
  if (line_by_line) {
    flags |= Flags::LINE_BY_LINE;
  }

  try {

    if (check) {
      if (stream) {
        std::cout << "----CHECK STREAM----" << std::endl;
        std::string pattern_ = read_from_file(pattern);
        std::fstream document_stream(document, std::ios::in | std::ios::binary);
        auto s_query =
            reql(pattern_, flags, max_mempool_duplications, max_deterministic_states, buffer_size);
        FStreamReader reader(document_stream);

        if (s_query.check(&reader)) {
          std::cout << "true" << std::endl;
        } else {
          std::cout << "false" << std::endl;
        }

      } else {
        std::cout << "----CHECK----" << std::endl;

        std::string pattern_ = read_from_file(pattern);
        std::string document_ = read_from_file(document);

        auto query = reql(pattern_, flags, max_mempool_duplications, max_deterministic_states);
        if (query.check(document)) {
          std::cout << "true" << std::endl;
        } else {
          std::cout << "false" << std::endl;
        }
      }
      return 0;
    }

    if (multi_spanners) {
      if (stream) {
        std::cout << "---MULTI STREAM----" << std::endl;
        std::string pattern_ = read_from_file(pattern);
        std::fstream document_stream(document, std::ios::in | std::ios::binary);
        auto s_query = multi_reql(pattern_, flags, max_mempool_duplications,
                                  max_deterministic_states, buffer_size);
        FStreamReader reader(document_stream);
        const auto match_generator = s_query.finditer(&reader);
        for (auto& match : match_generator) {
          std::cout << match << "\n";
          for (auto g : match.groups(0)) {
            std::cout << " " << g << std::endl;
          }
        }

      } else {
        std::cout << "--- MULTI ---" << std::endl;
        std::string pattern_ = read_from_file(pattern);
        std::string document_ = read_from_file(document);

        auto multi_query =
            multi_reql(pattern_, flags, max_mempool_duplications, max_deterministic_states);
        const auto multi_match_generator = multi_query.finditer(document_);
        for (const auto& multi_match : multi_match_generator) {
          std::cout << multi_match << "\n";
          for (auto g : multi_match.groups(0)) {
            std::cout << " " << g << std::endl;
          }
        }
      }

    } else if (stream) {
      std::cout << "----STREAM----" << std::endl;
      std::string pattern_ = read_from_file(pattern);
      std::fstream document_stream(document, std::ios::in | std::ios::binary);
      auto s_query =
          reql(pattern_, flags, max_mempool_duplications, max_deterministic_states, buffer_size);
      FStreamReader reader(document_stream);
      const auto match_generator = s_query.finditer(&reader);
      for (auto& match : match_generator) {
        std::cout << match << "\n";
        std::cout << " " << match.group(0) << std::endl;
      }
    } else if (findone) {
      std::cout << "--- FINDONE" << std::endl;
      std::string pattern_ = read_from_file(pattern);
      std::string document_ = read_from_file(document);

      auto query = reql(pattern_, flags, max_mempool_duplications, max_deterministic_states);

      try {
        auto match = query.findone(document_);
        std::cout << match << std::endl;
        std::cout << " " << match.group(0) << std::endl;
      } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
      }

    } else {
      std::cout << "--- STANDARD ---" << std::endl;
      std::string pattern_ = read_from_file(pattern);
      std::string document_ = read_from_file(document);

      auto query = reql(pattern_, flags, max_mempool_duplications, max_deterministic_states);
      const auto match_generator = query.finditer(document_);
      for (auto& match : match_generator) {
        std::cout << match << "\n";
        std::cout << " " << match.group(0) << std::endl;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <REmatch/REmatch.hpp>
#include <cli11.hpp>

using namespace REmatch;

// TODO: File input
// TODO: Const methods if possible
int main(int argc, char** argv) {
  std::string pattern;
  std::string document;

  uint_fast32_t max_mempool_duplications{DEFAULT_MAX_MEMPOOL_DUPLICATIONS};
  uint_fast32_t max_deterministic_states{DEFAULT_MAX_DETERMINISTIC_STATES};

  bool multi_spanners{false};
  bool line_by_line{false};

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

  app.add_flag("-m,--multi-spanners", multi_spanners)
      ->description("Enable Multi Spanners support");

  app.add_flag("-l,--line-by-line", line_by_line)
      ->description("Enable line by line");

  CLI11_PARSE(app, argc, argv);

  Flags flags = Flags::NONE;
  if (line_by_line) {
    flags |= Flags::LINE_BY_LINE;
  }

  try {
    if (multi_spanners) {
      auto multi_query = multi_reql(pattern, flags, max_mempool_duplications,
                                    max_deterministic_states);
      const auto multi_match_generator = multi_query.finditer(document);
      for (auto& multi_match : multi_match_generator) {
        std::cout << multi_match << "\n";
      }
    } else {
      auto query = reql(pattern, flags, max_mempool_duplications,
                        max_deterministic_states);
      const auto match_generator = query.finditer(document);
      for (auto& match : match_generator) {
        std::cout << match << "\n";
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

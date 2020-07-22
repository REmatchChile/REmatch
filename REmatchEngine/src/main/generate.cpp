#include "CLI11.hpp"
#include "rematch.hpp"
#include "util.hpp"

int main(int argc, char const *argv[]) {

  // COMMAND LINE INTERFACE

  CLI::App app{"REsample"};

  std::string script_path;
  CLI::Option *sopt = app.add_option("-s,--script_file,script_file",
                                     script_path,
                                     "Input script file")->check(CLI::ExistingFile);

  int n{0};
  app.add_option("-n,number", n, "Output length");

  std::string regex;
  app.add_option("-e,--expression", regex, "Input regex")->excludes(sopt);

  CLI11_PARSE(app, argc, argv);

  if(sopt->count())
    regex = file2str(script_path);

  rematch::RegEx regexp(regex);

  std::cout << regexp.uniformGenerate(n) << '\n';

  return 0;
}

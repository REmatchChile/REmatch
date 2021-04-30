#include <iostream>

#include "charclass.hpp"

using namespace rematch;

int main(int argc, char *argv[]) {

  CharClassBuilder ch1, ch2;

  ch1.add_range('e', 'l');

  std::cout << "Charclass1: " << ch1 << '\n';

  ch1.add_range('c', 'g');

  std::cout << "Charclass1: " << ch1 << '\n';

  ch1.add_range('r', 'v');
  std::cout << "Charclass1: " << ch1 << '\n';

  ch1.add_range('c', 'm');
  std::cout << "Charclass1: " << ch1 << '\n';

  ch1.add_range('m', '~');
  std::cout << "Charclass1: " << ch1 << '\n';

  ch1.add_range(' ', '/');
  std::cout << "Charclass1: " << ch1 << '\n';

  ch2.add_range('A', 'Z');
  ch2.add_range('a', 'z');
  ch2.add_range('0', '9');
  std::cout << "Charclass2: " << ch2 << '\n';

  CharClassBuilder *ch3 = ch1.intersect(&ch2);
  std::cout << "Charclass3: " << *ch3 << '\n';

  CharClassBuilder *ch4 = ch1.set_minus(&ch2);
  std::cout << "Charclass4: " << *ch4 << '\n';

  return 0;
}
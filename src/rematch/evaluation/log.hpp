#include <iostream>

// #define LOG_ENABLE

#ifdef LOG_ENABLE
#define LOG(x) (std::cout << x)
#else
#define LOG(x)
#endif

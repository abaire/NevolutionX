#ifndef NEVOLUTIONX_PBKIT_SDL_GPU_DEBUG_OUTPUT_H_
#define NEVOLUTIONX_PBKIT_SDL_GPU_DEBUG_OUTPUT_H_

#include <windows.h>
#include <string>

//#include "printf/printf.h"

#define ASSERT(c) \
  if (!(c)) { \
    PrintAssertAndWaitForever(#c, __FILE__, __LINE__); \
  }

template<typename... VarArgs>
inline void PrintMsg(const char* fmt, VarArgs&&... args) {
  int string_length = snprintf_(nullptr, 0, fmt, args...);
  std::string buf;
  buf.resize(string_length);

  snprintf_(&buf[0], string_length + 1, fmt, args...);
  DbgPrint("%s", buf.c_str());
}

void PrintAssertAndWaitForever(const char* assert_code, const char* filename, uint32_t line);

#endif // NEVOLUTIONX_PBKIT_SDL_GPU_DEBUG_OUTPUT_H_

#include "utility.h"

#include <cstdarg>
#include <cstdio>

FILE *s_log = nullptr;

bool CreateLog(const char *path)
{
  if (s_log)
    fclose(s_log);

  s_log = nullptr;
  return fopen_s(&s_log, path, "w") == 0;
}

void PrintLog(const char *format, ...)
{
  if (!s_log)
    return;

  va_list args;
  va_start(args, format);
  vfprintf(s_log, format, args);
  va_end(args);

  fputc('\n', s_log);
  fflush(s_log);
}

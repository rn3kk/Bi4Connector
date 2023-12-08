#include "Msg.h"

std::string Msg::to_string()
{
  char str[800];
  int len =
      snprintf(str, 800, "MSG: dir=%d, cmd=%d, value=%s", dir, cmd, value.c_str());
  return std::string(str, len);
}

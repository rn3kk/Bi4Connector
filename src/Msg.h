#ifndef MSG_H
#define MSG_H
#include <string>

struct Msg
{
  int8_t dir;
  int16_t cmd;
  std::string value;
  std::string to_string();
};

#endif // MSG_H

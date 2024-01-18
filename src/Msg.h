#ifndef MSG_H
#define MSG_H
#include <msgpack.hpp>
#include <msgpack/adaptor/define_decl.hpp>
#include <msgpack/v3/object_fwd_decl.hpp>
#include <msgpack/v3/pack_decl.hpp>
#include <ostream>
#include <string>

struct Msg
{
  Msg(const msgpack::object_array &array);
  int8_t dir;
  int16_t cmd;
  std::string_view value;
  std::string raw;
};

std::ostream &operator<<(std::ostream &os, const Msg &msg);

#endif // MSG_H

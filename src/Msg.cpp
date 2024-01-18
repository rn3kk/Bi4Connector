#include "Msg.h"

std::ostream &operator<<(std::ostream &os, const Msg &msg)
{
  os << "MSG: dir=" << msg.dir << ", cmd=" << msg.cmd << ", value=" << msg.value;
  return os;
}

Msg::Msg(const msgpack::object_array &array)
{
  dir = array.ptr[0].as<int8_t>();
  cmd = array.ptr[1].as<int16_t>();
  value = array.ptr[2].as<std::string_view>();

  std::stringstream ss;
  msgpack::packer<std::stringstream> packer(&ss);
  packer.pack_array(3).pack_int8(dir).pack_int16(cmd).pack(value);
  raw = ss.str();
}

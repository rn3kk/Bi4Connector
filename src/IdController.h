#ifndef IDCONTROLLER_H
#define IDCONTROLLER_H

#include <cstdint>
#include <set>
#include <string>

class IdController
{
public:
  static const IdController&  getInstance();
  ~IdController();
  uint16_t getId();
  void returnBackId(uint16_t id);

private:
  IdController();

private:
  std::set<uint16_t> m_idList;
};

#endif // IDCONTROLLER_H

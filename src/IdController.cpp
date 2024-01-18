#include "IdController.h"
#include <cstring>
#include "Logger.h"

IdController::IdController()
{

  for(uint16_t i = 0xFF +1; i < UINT16_MAX; ++i)
  {
    m_idList.insert(i);
  }
}

const IdController &IdController::getInstance()
{
  static IdController instance;
  return instance;
}

IdController::~IdController()
{

}

uint16_t IdController::getId()
{
  if (m_idList.begin() != m_idList.end())
  {
    uint16_t id = *m_idList.begin();
    m_idList.erase(m_idList.begin());
    return id;
  }
  //  toStatusLog("Error!!! ID list is empty!");
  return 0;
}

void IdController::returnBackId(uint16_t id)
{
  m_idList.insert(id);
}

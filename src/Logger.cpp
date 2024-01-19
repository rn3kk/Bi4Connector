#include "Logger.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <unistd.h>
using namespace std;

void lDebug(int queNum, std::string_view msg)
{
  std::string str = std::to_string(queNum) + " ";
  str.append(msg.data(), msg.size());
  syslog(LOG_DEBUG, "%s", str.c_str());
}

void lInfo(int queNum, std::string_view msg)
{
  std::string str = std::to_string(queNum) + " ";
  str.append(msg.data(), msg.size());
  syslog(LOG_INFO, "%s", str.c_str());
}

void lError(int queNum, std::string_view msg)
{
  std::string str = std::to_string(queNum) + " ";
  str.append(msg.data(), msg.size());
  syslog(LOG_ERR, "%s", str.c_str());
}

void log(int priority, std::string_view message)
{
  switch (priority)
  {
  case 0:
    sd_journal_print(LOG_DEBUG, "%s", message.data());
    break;
  case 2:
    sd_journal_print(LOG_ERR, "%s", message.data());
    break;
  }
}

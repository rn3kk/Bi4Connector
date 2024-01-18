#include "Logger.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <syslog.h>
#include <unistd.h>
using namespace std;

void lDebug(int queNum, std::string_view msg)
{
  syslog(LOG_DEBUG, (std::to_string(queNum) + " " + msg.data()).c_str());
}

void lInfo(int queNum, std::string_view msg)
{
  syslog(LOG_INFO, (std::to_string(queNum) + " " + msg.data()).c_str());
}

void lError(int queNum, std::string_view msg)
{
  syslog(LOG_ERR, (std::to_string(queNum) + " " + msg.data()).c_str());
}

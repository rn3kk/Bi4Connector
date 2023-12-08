#include "Logger.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <syslog.h>
#include <unistd.h>
using namespace std;

static queue<string> * log_queue = nullptr;
queue<string> consoleQueue;
static int queueCount = 0;
static pthread_t logThreadId = 0;
static bool terminate = false;

void *loggerThread(void* vargp);

void initLogger(int count_threads)
{
  log_queue = new queue<string>[count_threads];
  queueCount = count_threads;
  pthread_create(&logThreadId, 0x0, loggerThread, 0x0);
}

void destroyLogger()
{
  if (log_queue){
    delete [] log_queue;
    log_queue = nullptr;
  }
  if(logThreadId){
    ::terminate = true;
    pthread_join(logThreadId, 0x0);
  }
}


void addMessage(int queNum, std::string msg)
{
  syslog(LOG_DEBUG, std::string(std::to_string(queNum) + " " + msg).c_str());
  return;

  if (queNum > queueCount)
    return;
  log_queue[queNum].push(msg);
}

void *loggerThread(void* vargp)
{
  ofstream f;
  f.open ("bi4connector.log", std::ios_base::app);

  while(!::terminate)
  {
    for(int i=0; i< queueCount; ++i){
      if (log_queue[i].empty())
        continue;
      string data = log_queue[i].front();
      log_queue[i].pop();
      if(data.length() >0){
        f << i << " " << data << "\n";
      }
    }
    f.flush();
    if(!consoleQueue.empty())
    {
      string data = consoleQueue.front();
      consoleQueue.pop();
      printf(data.c_str());
    }
    usleep(1000);
  }
  f.close();
  return nullptr;
}

void lDebug(int queNum, std::string msg)
{
  addMessage(queNum, string("[DEBUG] ") + msg);
}

void lInfo(int queNum, std::string msg)
{
  addMessage(queNum, string("[INFO] ") + msg);
}

void lError(int queNum, std::string msg)
{
  addMessage(queNum, string("[ERROR] ") + msg);
}

void lConsole(std::string msg)
{
  consoleQueue.push(msg);
}

std::mutex mutexErrorLog;
void toStatusLog(int thId, std::string msg)
{
  toStatusLog(to_string(thId) + " " + msg);
}

void toStatusLog(std::string msg)
{
  mutexErrorLog.lock();
  ofstream f;
  f.open ("bi4connector_status.log", std::ios_base::app);
  f << msg << "\n";
  f.close();
  mutexErrorLog.unlock();
}

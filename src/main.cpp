#include "Logger.h"
//#include "Peer.h"
#include "Server.h"
//#include <algorithm>

//#include <cstring>


#include <list>

#include <pthread.h>
#include <set>
#include <stdio.h>

using namespace std;

extern void *epoll_server(void *vargp);
extern void intit_server();

int main(int argc, char *argv[])
{  
//  std::vector<char> s;
//  for(int i = 0; i< 5; ++i)
//  {
//    s.push_back('G');
//  }
//  char *str = "Hello world";
//  int len = strlen(str);
//  int nowSize = s.size();
//  s.resize(nowSize + len);

//  memcpy(s.data() + nowSize, str, len);

  int n = 3;
  intit_server(n);
  pthread_t serverThreadId[n];
  for (size_t i = 0; i < n; i++)
  {
    toStatusLog("Create thread " + to_string(i));
    pthread_create(serverThreadId + i, 0x0, epoll_server, (void *)i);
  }
  for (int i = 0; i < n; i++)
  {
    toStatusLog("Join thread " + to_string(i));
    pthread_join(serverThreadId[i], 0x0);
    lDebug(i, "Thread id is join end");
  }
  lInfo(0, "App is end");
  destroyLogger();
  return (0);
}

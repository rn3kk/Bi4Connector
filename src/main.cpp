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

void *epoll_server(void *vargp);
void *input_connection_server_handler(void *vargp);
void intit_server();

int main(int argc, char *argv[])
{
  intit_server();

  int n = 9;
  pthread_t input_connectio_thread;
  pthread_create(&input_connectio_thread, 0x0, input_connection_server_handler, 0);

  pthread_t serverThreadId[n];
  for (size_t i = 0; i < n; i++)
  {
    lInfo(i, "Create thread");
    pthread_create(serverThreadId + i, 0x0, epoll_server, (void *)i);
  }
  for (int i = 0; i < n; i++)
  {
    lInfo(i, "Join thread");
    pthread_join(serverThreadId[i], 0x0);
    lDebug(i, "Thread id is join end");
  }
  lInfo(0, "App is end");
  //  destroyLogger();
  return (0);
}

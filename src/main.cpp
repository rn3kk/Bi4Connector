#include "Peer.h"

#include <pthread.h>
#include <sys/epoll.h>

#include "Logger.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <list>
#include <msgpack/unpack.hpp>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <set>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define check(expr) \
  if (!(expr)) { \
    perror(#expr); \
    kill(0, SIGTERM); \
  }

using namespace std;

int epoll_listen = 0;
int epoll_read = 0;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

bool terminate = false;

#define MAX_EVENTS 10

void setnonblocking(int fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

void *epoll_server(void *vargp)
{
  std::list<Peer *> peerList;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3333);
  addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t adr_len = sizeof(addr);

  struct epoll_event ev;
  struct epoll_event events[MAX_EVENTS];
  int listen_sock, conn_sock, nfds, epollfd;
  listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0)
  {
    perror("socket");
    exit(1);
  }

  const int enable = 1;
  if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");

  if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(2);
  }

  if (listen(listen_sock, 100) < 0)
  {
    perror("listen");
    exit(3);
  }

  fcntl(listen_sock, F_SETFL, O_NONBLOCK);

  epollfd = epoll_create(1024);
  if (epollfd == -1)
  {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN | EPOLLERR;
  ev.data.fd = listen_sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
  {
    perror("epoll_ctl: listen_sock");
    exit(EXIT_FAILURE);
  }

  struct sockaddr in_addr;
  socklen_t in_addr_len = sizeof(in_addr);

  for (;;)
  {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
//    toStatusLog(tId, "Have event");
    for (int n = 0; n < nfds; ++n) {
      if (events[n].data.fd == listen_sock) {
        int conn_sock = accept(listen_sock, (struct sockaddr *) &in_addr, &in_addr_len);
        if (conn_sock >= 0)
        {
          char *ip = inet_ntoa(in_addr.sin_addr);
          uint16_t port = htons(in_addr.sin_port);
          lInfo(tId, "Input connection " + string(ip) + " " + to_string(port));

    for (int n = 0; n < nfds; ++n)
    {
      if (events[n].data.fd == listen_sock)
      {
        conn_sock = accept(listen_sock, (struct sockaddr *)&in_addr, &in_addr_len);
        if (conn_sock == -1)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }
        setnonblocking(conn_sock);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = conn_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
        {
          perror("epoll_ctl: conn_sock");
          exit(EXIT_FAILURE);
        }
        peerList.push_back(new Peer(conn_sock, epollfd));
      }
      else
      {
        char buf[512];
        int rbytes = read(events[n].data.fd, &buf, 512);
        printf("read bytes %d\n", rbytes);
        send(events[n].data.fd, &buf, rbytes, 0x0);
      }
    }
  }
  return 0x0;
}

int main(int argc, char *argv[])
{
  pthread_t serverThreadId;
  pthread_create(&serverThreadId, 0x0, epoll_server, 0x0);
  pthread_join(serverThreadId, 0x0);
  return (0);
}

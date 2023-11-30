#include "Logger.h"
#include "Peer.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <list>
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

#define check(expr)                                                                 \
  if (!(expr))                                                                      \
  {                                                                                 \
    perror(#expr);                                                                  \
    kill(0, SIGTERM);                                                               \
  }

using namespace std;

int epoll_listen = 0;

int listen_sock = 0;
int epollfd = 0;

void error(const char *msg)
{
  toStatusLog(string(msg) + " " + std::strerror(errno));
  exit(1);
}

bool terminate = false;

#define MAX_EVENTS 10

void setnonblocking(int fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

void enable_keepalive(int sock)
{
  int yes = 1;
  check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

  int idle = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

  int interval = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

  int maxpkt = 6;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
}

void *epoll_server(void *vargp)
{
  std::list<Peer *> peerList;
  int tId = (size_t)vargp;
  lDebug(tId, "Thread is start: " + to_string(tId));
  toStatusLog(tId, "Start thread");

  struct epoll_event events[MAX_EVENTS];
  struct epoll_event ev = {0};
  ev.events = EPOLLIN | EPOLLEXCLUSIVE;
  ev.data.fd = listen_sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
  {
    error("epoll_ctl: listen_sock");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in in_addr;
  socklen_t in_addr_len = sizeof(in_addr);

  for (;;)
  {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
      error("epoll_wait");
      exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; ++n)
    {
      if (events[n].events & EPOLLIN)
      {
        if (events[n].data.fd == listen_sock) // accept input connection
        {
          int conn_sock =
              accept(listen_sock, (struct sockaddr *)&in_addr, &in_addr_len);
          if (conn_sock >= 0)
          {
            char *ip = inet_ntoa(in_addr.sin_addr);
            uint16_t port = htons(in_addr.sin_port);
            lInfo(tId, "Input connection " + string(ip) + " " + to_string(port));

            setnonblocking(conn_sock);
            enable_keepalive(conn_sock);
            Peer *peer = new Peer(conn_sock, epollfd);
            peerList.push_back(peer);
            ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLONESHOT;
            ev.data.ptr = peer;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
            {
              perror("epoll_ctl: conn_sock");
              exit(EXIT_FAILURE);
            }
          }
          else if (errno == EAGAIN)
          {
            lError(tId, "Server thread ***FAILED*** to proccess accept with EAGAIN");
          }
        }
        else
        {
          char buf[512];
          while (1)
          {
            int rbytes = read(((Peer *)events[n].data.ptr)->sock(), &buf, 512);
            if (rbytes > 0)
            {

              //              send(((Peer *)events[n].data.ptr)->sock(), &buf,
              //              rbytes, 0x0);
            }
            if (errno == EAGAIN)
            {
              ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
              ev.data.ptr = events[n].data.ptr;
              if (epoll_ctl(epollfd, EPOLL_CTL_MOD,
                            ((Peer *)events[n].data.ptr)->sock(), &ev) == -1)
              {
                error("epoll_ctl: events[n].data.fd");
                exit(EXIT_FAILURE);
              }
              break;
            }
          }
        }
      }
      else if (events[n].events & (EPOLLRDHUP | EPOLLHUP))
      {
        lInfo(tId, "connection closed ");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL);
        close(events[n].data.fd);
      }
    }
  }
  return 0x0;
}

int main(int argc, char *argv[])
{

  int n = 6;
  initLogger(n);
  toStatusLog("App is start");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3333);
  addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t adr_len = sizeof(addr);

  toStatusLog("Next start listen port");
  listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0)
  {
    error("socket");
    exit(1);
  }

  const int enable = 1;
  if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");

  toStatusLog("Next start bind port");
  if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    error("bind");
    exit(2);
  }

  if (listen(listen_sock, 3) < 0)
  {
    error("listen");
    exit(3);
  }

  fcntl(listen_sock, F_SETFL, O_NONBLOCK);

  epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    error("epoll_create1");
    exit(EXIT_FAILURE);
  }

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

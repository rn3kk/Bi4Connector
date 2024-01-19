#include "Logger.h"
#include "Peer.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/poll.h>

// #include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <list>
#include <unistd.h>
#define MAX_EVENTS 10

using namespace std;

static int epollfd = 0;
bool neet_terminate = false;

void setnonblocking(int fd);
void enable_keepalive(int sock);
void error(const char *msg);

#define check(expr)                                                                 \
  if (!(expr))                                                                      \
  {                                                                                 \
    perror(#expr);                                                                  \
    kill(0, SIGTERM);                                                               \
  }

void intit_server()
{
  epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    error("epoll_create1");
  }
}

void *input_connection_server_handler(void *vargp)
{
  lInfo(-1, "App is start");
  int port = 7002;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t adr_len = sizeof(addr);

  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0)
  {
    error("socket");
  }

  const int enable = 1;
  if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");

  mDebug(-1, "Next start bind port");
  if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    error("bind");
  }

  if (listen(listen_sock, 35) < 0)
  {
    error("listen");
  }
  fcntl(listen_sock, F_SETFL, O_NONBLOCK);

  lInfo(-1, "Start listen port: " + std::to_string(port));

  struct pollfd fds;
  fds.events = POLL_IN;
  fds.fd = listen_sock;

  while (!neet_terminate)
  {
    int pollResult = poll(&fds, 1, 1000);
    if (pollResult > 0)
    {
      if (fds.revents & POLL_IN)
      {
        struct sockaddr_in in_addr;
        socklen_t addrlen = sizeof(in_addr);

        int client_socket =
            accept(listen_sock, (struct sockaddr *)&in_addr, &addrlen);
        char *ip = inet_ntoa(in_addr.sin_addr);
        uint16_t port = htons(in_addr.sin_port);
        lInfo(-1, "Input connection " + string(ip) + " " + to_string(port));

        setnonblocking(client_socket);
        enable_keepalive(client_socket);
        Peer *peer = new Peer(client_socket, epollfd, ip, port);
        struct epoll_event ev1 = {0};
        ev1.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLONESHOT;
        ev1.data.ptr = peer;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket, &ev1) == -1)
        {
          perror("epoll_ctl: conn_sock");
          exit(EXIT_FAILURE);
        }
        lDebug(-1, "Add event to epoll for read socket data");
      }
      if (fds.revents & POLL_HUP)
      {
        lInfo(-1, "Server is closed");
        exit(0);
      }
    }
    else if (pollResult < 0)
    {
      lError(-1, "Poll filed. Input connectio thread is stopped");
      break;
    }
    else if (pollResult == 0)
    {
      // poll timeout
      continue;
    }
  }
  return 0x0;
}

void *epoll_server(void *vargp)
{
  int tId = (size_t)vargp;
  lDebug(tId, "Start thread");

  struct epoll_event events[MAX_EVENTS];
  for (;;)
  {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
      if (errno != EINTR)
        error("epoll_wait");
      // exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; ++n)
    {
      if (events[n].events & EPOLLIN)
      {
        char buf[4096];
        //          lDebug(tId, "read start! sock: " +
        //                          std::to_string(((Peer
        //                          *)events[n].data.ptr)->sock()));
        int rbytes = 0;
        do
        {
          rbytes = read(((Peer *)events[n].data.ptr)->sock(),
                        ((Peer *)events[n].data.ptr)->getBufferPtr(),
                        ((Peer *)events[n].data.ptr)->getBufferLen());
          if (rbytes >= 0)
          {
            ((Peer *)events[n].data.ptr)->handleReceivedData(rbytes, tId);
          }
          else
          {
            if (errno == EAGAIN)
            {
              struct epoll_event ev1 = {0};
              ev1.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
              ev1.data.ptr = events[n].data.ptr;
              if (epoll_ctl(epollfd, EPOLL_CTL_MOD,
                            ((Peer *)events[n].data.ptr)->sock(), &ev1) == -1)
              {
                error("epoll_ctl: events[n].data.fd");
                exit(EXIT_FAILURE);
              }
            }
            else
              lDebug(tId, "123 errno: " + std::to_string(errno));
            break;
          }
        }
        while (rbytes > 0);
      }
      if (events[n].events & (EPOLLRDHUP | EPOLLHUP))
      {
        lInfo(tId, "connection closed " +
                       std::to_string(((Peer *)events[n].data.ptr)->sock()));
        epoll_ctl(epollfd, EPOLL_CTL_DEL, ((Peer *)events[n].data.ptr)->sock(),
                  NULL);
        close(((Peer *)events[n].data.ptr)->sock());
      }
    }
  }
  return 0x0;
}

void setnonblocking(int fd) { fcntl(fd, F_SETFL, O_NONBLOCK); }

void enable_keepalive(int sock)
{
  int yes = 1;
  check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

  int idle = 15;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

  int interval = 5;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

  int maxpkt = 10;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
}

void error(const char *msg)
{
  mError(-1, msg << " " << std::strerror(errno));
  exit(1);
}

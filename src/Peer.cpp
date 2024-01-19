#include "Peer.h"
#include "Logger.h"
#include "Session.h"
#include <msgpack.hpp>
#include <msgpack/adaptor/define_decl.hpp>
#include <msgpack/v3/object_fwd_decl.hpp>
#include <msgpack/v3/pack_decl.hpp>
#include <string_view>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define ID_LENGTH 10

const int DIR_TO_BI4CONNECTOR = 9;
const int CMD_LIC_HASH = 1;
const int CMD_CLIENT_TYPE = 2;
const int MSGPACK_BUF_LEN = 4096 * 10;
const char *PEER = "Peer";

extern Session *getSession(std::string sessId);

Peer::Peer(int sock, int epollFd, char *ip, uint16_t port)
{
  m_sock = sock;
  m_epollFd = epollFd;
  m_receivedData.reserve(1024 * 1024 * 2);
  m_unpack.reserve_buffer(MSGPACK_BUF_LEN);
  m_ip = std::string(ip);
  m_port = port;
}

Peer::~Peer() {}

int Peer::sock() const
{
  return m_sock;
}

void Peer::handleReceivedData(int len, int threadId)
{  
  m_unpack.buffer_consumed(len);
  msgpack::object_handle oh;
  while (m_unpack.next(oh))
  {
    if (oh->type == msgpack::type::ARRAY)
    {
      Msg msg(oh->via.array);
      handleMessage(msg, threadId);
    }
  }
}

char *Peer::getBufferPtr()
{
  m_unpack.reserve_buffer(MSGPACK_BUF_LEN);
  return m_unpack.buffer();
}

int Peer::getBufferLen() { return MSGPACK_BUF_LEN; }

void Peer::updatePeerType(char type)
{
  switch (type) {
  case RADIO_BOX:
    m_type = RADIO_BOX;
    break;  
  case PC_CLIENT:
    m_type = PC_CLIENT;
    break;  
  case MOBILE_CLIENT:
    m_type = MOBILE_CLIENT;
    break;  
  default:
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, m_sock, nullptr);
    lDebug(0, "next close socket " + std::to_string(m_sock));
    close(m_sock);
    break;
  }
}

void Peer::handleMessage(const Msg &msg, int threadId)
{
  lDebug(threadId, "receive msg " + msg.raw);

  if (msg.dir == DIR_TO_BI4CONNECTOR)
  {
    if (msg.cmd == CMD_LIC_HASH){
      if(!m_session){
        m_session = getSession(msg.value);
      }
    }else if (msg.cmd == CMD_CLIENT_TYPE){
      int client_type = atoi(msg.value.data());
      if (client_type == RADIO_BOX)
      {
        m_type = RADIO_BOX;
        lInfo(threadId, "Connected RADIO_BOX from ip: " + m_ip +
                            " port: " + std::to_string(m_port));
        m_session->m_radio = this;
      }
      else if (client_type == PC_CLIENT)
      {
        m_type = PC_CLIENT;
        lInfo(threadId, "Connected PC_CLIENT from ip: " + m_ip +
                            " port: " + std::to_string(m_port));
        m_session->m_client = this;
      }
      else
      {
        lError(threadId, "Unknown Input connection from ip: " + m_ip +
                             " port: " + std::to_string(m_port));
        m_type = UNKNOWN;
      }
    }
  }
  else if (m_session)
  {
    if(m_session->m_client == this){
      if(m_session->m_radio){
        write(m_session->m_radio->sock(), msg.raw.c_str(), msg.raw.size());
      }
      else
      {
        lError(threadId,
               (std::string("no radio for send data: ") + msg.value.data()).c_str());
      }
    }
    else if (m_session->m_radio == this)
    {
      if(m_session->m_client){
        write(m_session->m_client->sock(), msg.raw.c_str(), msg.raw.size());
      }
      else
      {
        mError(threadId, "no client for send data: " << msg.value);
      }
    }
  }
  else
  {
    lError(threadId,
           "No session for ip: " + m_ip + " port: " + std::to_string(m_port));
  }
}

void Peer::sendDataToRemotePeer(char *buf, int len) {}

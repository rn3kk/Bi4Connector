#include "Peer.h"
#include "Logger.h"
#include "Session.h"
#include <msgpack.hpp>
#include <msgpack/adaptor/define_decl.hpp>
#include <msgpack/v3/object_fwd_decl.hpp>
#include <msgpack/v3/pack_decl.hpp>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define ID_LENGTH 10

const int DIR_TO_BI4CONNECTOR = 9;
const int CMD_LIC_HASH = 1;
const int CMD_CLIENT_TYPE = 2;
const int MSGPACK_BUF_LEN = 4096 * 10;
const char *PEER = "Peer";

extern Session* getSession(std::string sessId);

Peer::Peer(int sock, int epollFd)
{
  m_sock = sock;
  m_epollFd = epollFd;
  m_receivedData.reserve(1024 * 1024 * 2);
  m_unpack.reserve_buffer(MSGPACK_BUF_LEN);
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
    if (oh->type == msgpack::type::ARRAY) {
      msgpack::object_array array = oh->via.array;
      Msg msg;
      msg.dir = array.ptr[0].as<int8_t>();
      msg.cmd = array.ptr[1].as<int16_t>();
      msg.value = array.ptr[2].as<std::string>();
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

void Peer::handleMessage(Msg msg, int threadId)
{
  lDebug(threadId, "receive msg ");
  lDebug(threadId, msg.to_string());

  if (msg.dir == DIR_TO_BI4CONNECTOR) {
    if (msg.cmd == CMD_LIC_HASH){
      if(!m_session){
        m_session = getSession(msg.value);
      }
    }else if (msg.cmd == CMD_CLIENT_TYPE){
      int client_type = atoi(msg.value.c_str());
      if(client_type == RADIO_BOX)
        m_type = RADIO_BOX;
      else if(client_type == PC_CLIENT){
        m_type = PC_CLIENT;
      }
      else
        m_type = UNKNOWN;
    }
  } else if (m_session) {
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> packer(sbuf);
    packer.pack_array(3).pack_int8(msg.dir).pack_int16(msg.cmd).pack(msg.value);

    if(m_session->m_client == this){
      if(m_session->m_radio){
//        write(m_session->m_radio->sock())
      }
    } else if(m_session->m_radio == this){
      if(m_session->m_client){
//        write(m_session->m_client->sock())
      }
    }
  }
}

void Peer::sendDataToRemotePeer(char *buf, int len) {}



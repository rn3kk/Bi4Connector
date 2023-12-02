#include "Session.h"
#include "IdController.h"
#include "Logger.h"
#include <list>
#include <map>

//list<Session *> sesions;
static map<string, Session *> sessions;
static std::mutex sessions_edit_mutex;

Session::Session(string radioBoxHash)
{
  m_sessionId = radioBoxHash;
  lInfo(0, "Session() id " + m_sessionId);
}

Session::~Session()
{
  lInfo(0, "~Session() id " + m_sessionId);
}

std::string Session::sessionId() const
{
  return m_sessionId;
}

Session *getSession(std::string sesId)
{
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);
  try {
    return sessions.at(sesId);
  } catch (const std::out_of_range &) {
    Session *s = new Session(sesId);
    sessions[sesId] = s;
    return s;
  }
}

void removeFDFromSession(int fd, Session *session)
{
  if (session == nullptr)
    return;

  if (session->m_client->sock() == fd) {
    delete session->m_client;
    session->m_client = nullptr;
  } else if (session->m_radio->sock() == fd) {
    delete session->m_radio;
    session->m_radio = nullptr;
  }
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);

  if (session->m_radio == nullptr && session->m_client == nullptr) {
    for (auto it = sessions.begin(); it != sessions.end(); ++it) {
      if (it->second == session) {

        delete it->second;
        it = sessions.erase(it);
      }
    }
  }
}

void removeFd(int fd)
{
  if (fd == 0)
    return;
  std::lock_guard<std::mutex> guard(sessions_edit_mutex);
  for (auto i = sessions.begin(); i!= sessions.end(); ++i){
    Session * session = i->second;
    if(session->m_client!= nullptr && session->m_client->sock() == fd){
      delete session->m_client;
      session->m_client = nullptr;
    } else if (session->m_radio != nullptr && session->m_radio->sock() == fd){
      delete session->m_radio;
      session->m_radio = nullptr;
    }

    if (session->m_radio == nullptr && session->m_client == nullptr) {
      for (auto it = sessions.begin(); it != sessions.end(); ++it) {
        if (it->second == session) {
          delete it->second;
          it = sessions.erase(it);
        }
      }
    }
  }
}

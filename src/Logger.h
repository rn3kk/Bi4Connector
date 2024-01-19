#ifndef LOGGER_H
#define LOGGER_H
#include<string>

void lDebug(int queNum, std::string_view msg);
void lInfo(int queNum, std::string_view msg);
void lError(int queNum, std::string_view msg);

void log(int priority, std::string_view message);

#define mDebug(qNum, msg)                                                           \
  {                                                                                 \
    std::stringstream stream;                                                       \
    stream << qNum << " ";                                                          \
    stream << msg;                                                                  \
    log(0, stream.str());                                                           \
  }
#define mError(qNum, msg)                                                           \
  {                                                                                 \
    std::stringstream stream;                                                       \
    stream << qNum << " ";                                                          \
    stream << msg;                                                                  \
    log(2, stream.str());                                                           \
  }
#endif // LOGGER_H

#ifndef LOGGER_H
#define LOGGER_H
#include<string>

void initLogger(int count_threads);
void destroyLogger();
//void addMessage(int queNum, std::string msg);
void lDebug(int queNum, std::string msg);
void lInfo(int queNum, std::string msg);
void lError(int queNum, std::string msg);
void lConsole(std::string msg);

void toStatusLog(std::string msg);
void toStatusLog(int thId, std::string msg);

#endif // LOGGER_H

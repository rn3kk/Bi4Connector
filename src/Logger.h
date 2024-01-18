#ifndef LOGGER_H
#define LOGGER_H
#include<string>

void lDebug(int queNum, std::string_view msg);
void lInfo(int queNum, std::string_view msg);
void lError(int queNum, std::string_view msg);

#endif // LOGGER_H

#ifndef LOG_HEADER
#define LOG_HEADER

#include <Arduino.h>


class Log
{
public:
	static bool Enabled;

	static void Begin();
	static void End();

	static void Print(String msg);
	static void Println(String msg = "");

private:
	static void WriteSerial(String msg, bool newline = true);
};

#endif
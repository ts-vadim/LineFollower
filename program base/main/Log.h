#ifndef LOG_HEADER
#define LOG_HEADER

#include <Arduino.h>


class Log
{
public:
	static void Begin();
	static void End();

	static void Print(String msg);
	static void Println(String msg = "");

	static void SetEnabled(bool enabled);
	static bool IsEnabled();

private:
	static bool m_Enabled;

	static void WriteSerial(String msg, bool newline = true);
};

#endif
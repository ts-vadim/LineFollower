#include "Log.h"

bool Log::m_Enabled = true;


void Log::Begin()
{
	Serial.begin(9600);
}

void Log::End()
{
	Serial.end();
}

void Log::Print(String msg)
{
	if (m_Enabled)
		WriteSerial(msg, false);
}

void Log::Println(String msg = "")
{
	if (m_Enabled)
		WriteSerial(msg, true);
}

void Log::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
}

bool Log::IsEnabled()
{
	return m_Enabled;
}

void Log::WriteSerial(String msg, bool newline = true)
{
	//if (Serial.availableForWrite() >= msg.length())
	if (newline)
		Serial.println(msg);
	else
		Serial.print(msg);
	//Serial.flush();
}

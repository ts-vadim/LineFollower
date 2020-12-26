#include "Log.h"

bool Log::Enabled = true;


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
	if (Enabled)
		WriteSerial(msg, false);
}

void Log::Println(String msg = "")
{
	if (Enabled)
		WriteSerial(msg, true);
}

void Log::WriteSerial(String msg, bool newline = true)
{
	//if (Serial.availableForWrite() >= msg.length())
	if (newline)
		Serial.println(msg);
	else
		Serial.print(msg);
}
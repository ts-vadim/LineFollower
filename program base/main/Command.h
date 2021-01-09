#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#include <Arduino.h>


class Command
{
public:
	const bool empty;
	const char* const name;
	const char* const helptext;
	bool callAnyway = false;
	
	Command(const char* _name, void (*_func)(Command& self))
		: name(_name), func(_func), helptext(""), empty((_func) ? false : true) {}

	Command(const char* _name, const char* _help, void (*_func)(Command& self))
		: name(_name), helptext(_help), func(_func), empty((_func) ? false : true) {}

	Command(const char* _name, const char* _help, bool _callAnyway, const void (*_func)(Command& self))
		: name(_name), helptext(_help), callAnyway(_callAnyway), func(_func), empty((_func) ? false : true) {}

	void Execute() { func(*this); }

private:
	const void (*func)(Command& self);
};

#endif
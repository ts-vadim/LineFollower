#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#include <Arduino.h>


class Command
{
public:
	const bool empty; 
	const String name;
	const String helptext;
	bool callAnyway = false;
	
	Command(String _name, void (*_func)(Command& self))
		: name(_name), func(_func), helptext(""), empty((_func) ? false : true) {}
	Command(String _name, String _help, void (*_func)(Command& self))
		: name(_name), helptext(_help), func(_func), empty((_func) ? false : true) {}
	Command(String _name, String _help, bool _callAnyway, void (*_func)(Command& self))
		: name(_name), helptext(_help), callAnyway(_callAnyway), func(_func), empty((_func) ? false : true) {}

	void Execute() { func(*this); }

private:
	const void (*func)(Command& self);
};

#endif
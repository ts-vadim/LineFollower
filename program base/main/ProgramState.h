#ifndef PROGRAMSTATE_HEADER
#define PROGRAMSTATE_HEADER

#include <Arduino.h>
#include "Log.h"


class ProgramState
{
public:
	enum State { STOPPED, WAITING, RUNNING };
	static constexpr int StatesCount = 3;
	static const char* StateNames[StatesCount];

	static State GetState() { return m_State; }
	static void SetState(State state) { m_State = state; Log::Println(String("State: ") + StateNames[m_State]); }

private:
	static State m_State;
};

#endif
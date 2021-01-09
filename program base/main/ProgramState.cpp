#include "ProgramState.h"


ProgramState::State ProgramState::m_State = ProgramState::WAITING;
const char* ProgramState::StateNames[] = { "STOPPED", "WAITING", "RUNNING" };
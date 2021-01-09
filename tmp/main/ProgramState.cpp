#include "ProgramState.h"


ProgramState::State ProgramState::m_State = ProgramState::WAITING;
const String ProgramState::StateNames[] = { "STOPPED", "WAITING", "RUNNING" };
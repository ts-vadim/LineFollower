#include "ProgramState.h"


ProgramState::State ProgramState::m_State = ProgramState::WAITING;
const char* ProgramState::StateNames[] = { "STOPPED", "WAITING", "RUNNING" };


void ProgramState::SetState(State state)
{
    if (state != m_State)
    {
        // add something like OnStateChanged event
    }
    m_State = state;
    Log::Println(String("State: ") + StateNames[m_State]);
}
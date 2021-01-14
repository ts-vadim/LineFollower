#include <Arduino.h>
#include "Command.h"
#include "ProgramState.h"
#include "Log.h"
#include "strcpy_s.h"


void ProcessUserCommand(const char* cmdName, Command* cmds)
{
	bool givenFunctionWasCalled = false;
	Command *command = cmds;
	while (!command->empty)
	{
		if (strcmp(command->name, cmdName) == 0)
		{
			command->Execute();
			givenFunctionWasCalled = true;
			break;
		}
		else if (command->callAnyway)
		{
			command->Execute();
		}
		command++;
	}
	if (!givenFunctionWasCalled)
	{
		Log::Print("Undefined command \"");
		Log::Print(cmdName);
		Log::Println("\"");
	}
}


bool ReadUserInput(char* cmd, int cmdlen)
{
	if (Serial.available() > 0)
	{
		if (cmdlen <= 0)
			return true;
		
		String inp = Serial.readStringUntil('\n');
		if (inp != "")
		{
			int err = strcpy_s(cmd, cmdlen, inp.c_str());
			if (err == ERROR_SLIMITEXCEEDED)
			{
				Log::Print("Maximum input string length is ");
				Log::Println(String(cmdlen - 1));
			}
			return true;
		}
	}
	return false;
}


void UpdateStateCommands(const char* stateName, Command* cmds)
{
	Command *command = cmds;
	while (!command->empty)
	{
		if (strcmp(stateName, command->name) == 0 || command->callAnyway)
			command->Execute();	
		command++;
	}
}


Command* FindCommand(const char* name, Command* commands)
{
	for (Command* cmd = commands; !cmd->empty; cmd++)
		if (strcmp(name, cmd->name) == 0)
			return cmd;
	return nullptr;
}
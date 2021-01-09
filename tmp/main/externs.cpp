#include <Arduino.h>
#include "Command.h"
#include "ProgramState.h"
#include "Log.h"


void ProcessUserCommand(const char* cmdName, Command* cmds)
{
	bool typedFunctionCalled = false;
	Command *command = cmds;
	while (!command->empty)
	{
		if (strcmp(command->name, cmdName) == 0)
		{
			command->Execute();
			typedFunctionCalled = true;
			break;
		}
		else if (command->callAnyway)
		{
			command->Execute();
		}
		command++;
	}
	if (!typedFunctionCalled)
	{
		Log::Print("Undefined command \"");
		Log::Print(cmdName);
		Log::Println("\"");
	}
}

bool ReadUserCommand(char* cmd, bool wait)
{
	if (wait)
		while (Serial.available() <= 0);
	if (Serial.available() > 0)
	{
		String inp = Serial.readStringUntil('\n');
		if (inp != "")
		{
			strcpy(cmd, inp.c_str());
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
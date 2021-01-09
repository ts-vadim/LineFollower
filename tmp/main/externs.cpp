#include <Arduino.h>
#include "Command.h"
#include "ProgramState.h"
#include "Log.h"


void ProcessUserCommand(const String& cmdName, Command* cmds)
{
	bool typedFunctionCalled = false;
	Command *command = cmds;
	while (!command->empty)
	{
		if (cmdName == command->name)
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

bool ReadUserCommand(String& cmd, bool wait)
{
	if (wait)
		while (Serial.available() <= 0);
	if (Serial.available() > 0)
	{
		cmd = Serial.readStringUntil('\n');
		if (cmd != "")
			return true;
	}
	return false;
}


void UpdateStateCommands(const String& stateName, Command* cmds)
{
	Command *command = cmds;
	while (!command->empty)
	{
		if (stateName == command->name || command->callAnyway)
			command->Execute();	
		command++;
	}
}

Command* FindCommand(const String& name, Command* commands)
{
	for (Command* cmd = commands; !cmd->empty; cmd++)
		if (name == cmd->name)
			return cmd;
	return nullptr;
}
#include <Arduino.h>
#include "Command.h"
#include "ProgramState.h"
#include "Log.h"


void ProcessUserCommand(String cmd, Command* cmds)
{
	bool typedFunctionCalled = false;
	Command *command = cmds;
	while (!command->empty)
	{
		if (cmd.equals(command->name))
		{
			command->Execute();
			typedFunctionCalled = true;
		}
		else if (command->callAnyway)
		{
			command->Execute();
		}
		command++;
	}
	if (!typedFunctionCalled)
		Log::Println("Undefined command \"" + cmd + "\"");
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
		if (stateName.equals(command->name) || command->callAnyway)
		{
			Log::Println("Executing " + command->name);
			command->Execute();
		}
			
		command++;
	}
}

Command* FindCommand(String name, Command* commands)
{
	for (Command* cmd = commands; !cmd->empty; cmd++)
		if (cmd->name.equals(name))
			return cmd;
	return nullptr;
}

#include <Arduino.h>

#include "Dragster.h"
typedef TroykaLedMatrix Matrix;

#include "Log.h"
#include "ProgramState.h"
#include "Command.h"


//	###########  Functions  ###########
void stopped(Command&);
void waiting(Command&);
void running(Command&);

void PrintHelp(Command&);


//	###########  Externs  ###########
extern void ProcessUserCommand(const char* cmd, Command* cmds);
extern bool ReadUserCommand(char* cmd, int cmdlen, bool wait = false);
extern void UpdateStateCommands(const char* stateName, Command* cmds);
extern Command* FindCommand(const char* name, Command* commands);


//	###########  Commands  ###########
static Command userCommands[] = {
	Command("help", "prints this help message", PrintHelp),
	Command("stop", "sets state to STOP", [](Command&) { ProgramState::SetState(ProgramState::STOPPED); }),
	Command("wait", "sets state to WAIT", [](Command&) { ProgramState::SetState(ProgramState::WAITING); }),
	Command("run", "sets state to RUN", [](Command&) { ProgramState::SetState(ProgramState::RUNNING); }),
	Command("state", "prints current state",
		[](Command&) {
			Log::Print("Current state: ");
			Log::Println(ProgramState::StateNames[ProgramState::GetState()]);
		}
	),
	Command("states", "prints all states",
		[](Command&) {
			Log::Println("Known states:");
			for (int i = 0; i < ProgramState::StatesCount; i++)
			{
				Log::Print("  ");
				Log::Println(ProgramState::StateNames[i]);
			}
		}
	),
	Command("halt", "stops program and waits for user input", [](Command&) { ReadUserCommand(nullptr, 0, true); }),
	Command("reset", "resets arduino", [](Command&) { void (*reset)(void) = 0; reset(); }),
  Command("battery", "prints battery charge", [](Command&) { Log::Print("Battery charge: "); Log::Print(String(Dragster::GetBatteryCharge() * 100)); Log::Println("%"); }),
	Command("", nullptr)
};

static Command programCommands[] = {
	Command(ProgramState::StateNames[0], "stopped", stopped),
	Command(ProgramState::StateNames[1], "waiting", waiting),
	Command(ProgramState::StateNames[2], "running", running),
	Command("", nullptr)
};


//	##############################################################
//	##################      Arduino mains      ###################
//	##############################################################
void setup()
{
	Log::Begin();
	Log::Println("Program begins");

	FindCommand("help", userCommands)->Execute();

	char userCmd[11];
	while (true)
	{
		if (ReadUserCommand(userCmd, 11))
			ProcessUserCommand(userCmd, userCommands);
		UpdateStateCommands(ProgramState::StateNames[ProgramState::GetState()], programCommands);
	}
}

void loop() {}


//	###########  Function's Implementations  ###########
void stopped(Command&) {}
void waiting(Command&) {}
void running(Command&)
{
	static bool ledState = false;
	static const float cooldown = 0.1;
	static int prev = 0;

	int current = millis();

	if ((current - prev) / 1000.0 >= cooldown)
	{
		prev = current;
		digitalWrite(2, ledState);
		ledState = !ledState;
	}
}


void PrintHelp(Command&)
{
	Command *command = userCommands;

	Log::Println("User commands:");
	while (!command->empty)
	{
		Log::Print("  ");
		Log::Print(command->name);
		Log::Print(" - ");
		Log::Println(command->helptext);
		command++;
	}

	command = programCommands;
	Log::Println("State commands:");
	while (!command->empty)
	{
		Log::Print("  ");
		Log::Println(command->name);
		command++;
	}

	Log::Print("Current state: ");
	Log::Println(ProgramState::StateNames[ProgramState::GetState()]);
}



















//

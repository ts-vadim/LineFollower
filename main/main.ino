#include "Log.h"
#include "ProgramState.h"
#include "Command.h"



//  ###########  Global Variables  ###########
static const int morotPins[2] = { 5, 6 };
static const int sensorPins[2] = { 3, 4 };


//	###########  Functions  ###########
void stopped(Command&);
void waiting(Command&);
void running(Command&);

void PrintHelp(Command&);
void drive(int left, int right);

extern void ProcessUserCommand(String cmd, Command* cmds);
extern bool ReadUserCommand(String& cmd, bool wait = false);
extern void UpdateStateCommands(const String& stateName, Command* cmds);

extern Command* FindCommand(String name, Command* commands);


//	###########  Commands  ###########
static Command userCommands[] = {
	Command("help", "prints this help message", PrintHelp),
	Command("stop", "sets state to STOP", [](Command&) { ProgramState::SetState(ProgramState::STOPPED); }),
	Command("wait", "sets state to WAIT", [](Command&) { ProgramState::SetState(ProgramState::WAITING); }),
	Command("run", "sets state to RUN", [](Command&) { ProgramState::SetState(ProgramState::RUNNING); }),
	Command("state", "prints current state",
		[](Command&) {
			Log::Println("Current state: " + ProgramState::StateNames[ProgramState::GetState()]); 
		}
	),
	Command("states", "prints all states",
		[](Command&) {
			Log::Println("Known states:");
			for (int i = 0; i < ProgramState::StatesCount; i++) Log::Println("  " + ProgramState::StateNames[i]);
		}
	),
	Command("halt", "stop program mainloop", [](Command&) { while (true); }),
	Command("reset", "reset arduino", [](Command&) { void (*reset)(void) = 0; reset(); }),
	Command("", nullptr)
};

static Command programCommands[] = {
	Command(ProgramState::StateNames[0], stopped),
	Command(ProgramState::StateNames[1], waiting),
	Command(ProgramState::StateNames[2], running),
	Command("", nullptr)
};


//	######################  Arduino mains  ######################
void setup()
{
	Log::Begin();
	Log::Println("Program begins");

	Command* cmd = FindCommand("help", userCommands);
	if (cmd) cmd->Execute();

	long cur_time, prev_time = millis();
	float deltaTime = 1;
	while (true)
	{
		prev_time = cur_time;
		cur_time = millis();

		String userCmd;
		if (ReadUserCommand(userCmd))
			ProcessUserCommand(userCmd, userCommands);
		UpdateStateCommands(ProgramState::StateNames[ProgramState::GetState()], programCommands);

		deltaTime = (cur_time - prev_time) / 1000.0;
	}
}

void loop()
{
}


//	###########  Function's Implementations  ###########
void stopped(Command&) { Log::Println("Stopped"); }
void running(Command&) {}
void waiting(Command&)
{
	int sensors[2];

	sensors[0] = digitalRead(sensorPins[0]);
	sensors[1] = digitalRead(sensorPins[1]);

	drive((sensors[0] - sensors[1]) * 256.0, (sensors[1] - sensors[0]) * 256.0);

	Log::Println("Run");
}

void drive(int left, int right)
{
	analogWrite(morotPins[0], left);
	analogWrite(morotPins[1], right);

	Log::Print("Drive: ");
	Log::Print(String(left));
	Log::Print(" ");
	Log::Println(String(right));
}

void PrintHelp(Command& self)
{
	Log::Println("Help:");
	Command *command = userCommands;
	while (!command->empty)
	{
		Log::Print("  ");
		Log::Print(command->name);
		Log::Print((command->empty) ? "(empty)" : "");
		Log::Print(" - ");
		Log::Println(command->helptext);
		command++;
	}
}



















//

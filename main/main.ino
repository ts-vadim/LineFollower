//#include <TroykaLedMatrix.h>
//#include <PID_dragster.h>
//#include <Octoliner.h>
//#include <Dragster.h>
//#define Matrix TroykaLedMatrix




class Log;
class ProgramState;
class Command;


//	###########  Functions  ###########
void stopped(Command&);
void waiting(Command&);
void running(Command&);

void ProcessUserCommand(String cmd);
bool ReadUserCommand(String& cmd, bool wait = false);
void UpdateStateCommands();

void PrintHelp(Command& self);
void programLog(Command& self);



//	###########  Classes  ###########
//	----- Log
class Log
{
public:
	static bool Enabled;

	static void Begin() { Serial.begin(9600); }
	static void End() { Serial.end(); }

	static void Print(String msg)
	{
		if (Enabled)
			WriteSerial(msg, false);
	}
	static void Println(String msg = "")
	{
		if (Enabled)
			WriteSerial(msg, true);
	}

private:
	static void WriteSerial(String msg, bool newline = true)
	{
		//if (Serial.availableForWrite() >= msg.length())
		//{
			if (newline)
				Serial.println(msg);
			else
				Serial.print(msg);
		//}
	}
};
static bool Log::Enabled = true;

//	----- ProgramState
class ProgramState
{
public:
	enum State { STOPPED, WAITING, RUNNING };
	static const int StatesCount = 3;
	static const String StateNames[StatesCount];

	static State GetState() { return m_State; }
	static void SetState(State state) { m_State = state; Log::Println("State: " + StateNames[m_State]); }

private:
	static State m_State;
};
static ProgramState::State ProgramState::m_State = ProgramState::WAITING;
static const String ProgramState::StateNames[] = { "STOPPED", "WAITING", "RUNNING" };

//	----- Command
class Command
{
public:
	const bool empty; 
	const String name;
	const String helptext;
	bool callAnyway = false;
	
	Command(String _name, void (*_func)(Command& self))
		: name(_name), func(_func), helptext(""), empty((_func) ? false : true) {}
	Command(String _name, String _help, void (*_func)(Command& self))
		: name(_name), helptext(_help), func(_func), empty((_func) ? false : true) {}
	Command(String _name, String _help, bool _callAnyway, void (*_func)(Command& self))
		: name(_name), helptext(_help), callAnyway(_callAnyway), func(_func), empty((_func) ? false : true) {}

	void Execute() { func(*this); }

private:
	const void (*func)(Command& self);
};


//	###########  Variables  ###########
float deltaTime = 1;

Command programCommands[] = {
	Command(ProgramState::StateNames[0], stopped),
	Command(ProgramState::StateNames[1], waiting),
	Command(ProgramState::StateNames[2], running),
	Command("ProgramLog", "just a \"call anyway\" useless log function", programLog),
	Command("", nullptr)
};

Command userCommands[] = {
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
	Command("togglelog", "toggle useless log function",
		[](Command&) {
			programCommands[3].callAnyway = !programCommands[3].callAnyway; programCommands[3].Execute();
		}
	),
	Command("halt", "stop program mainloop", [](Command&) { while (true); }),
	Command("reset", "reset arduino", [](Command&) { void (*reset)(void) = 0; reset(); }),
	Command("", nullptr)
};


//	###########  Arduino mains  ###########
void setup()
{
	Log::Begin();
	Log::Println("Program begins");
	userCommands[0].Execute(); // call PrintHelp()

	long cur_time, prev_time = millis();
	while (true)
	{
		prev_time = cur_time;
		cur_time = millis();

		String userCmd;
		if (ReadUserCommand(userCmd))
			ProcessUserCommand(userCmd);
		UpdateStateCommands();

		deltaTime = (cur_time - prev_time) / 1000.0;
	}
}

void loop()
{
}


//	###########  Function's Implementations  ###########
void stopped(Command&) {}
void waiting(Command&) {}
void running(Command&) {}

void ProcessUserCommand(String cmd)
{
	bool typedFunctionCalled = false;
	Command *command = &userCommands[0];
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


void UpdateStateCommands()
{
	const String& stateName = ProgramState::StateNames[ProgramState::GetState()];
	Command *command = &programCommands[0];
	while (!command->empty)
	{
		if (stateName.equals(command->name) || command->callAnyway)
			command->Execute();
		command++;
	}
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


void programLog(Command& self)
{
	Log::Println(self.name + ": delta = " + deltaTime);
}






















//

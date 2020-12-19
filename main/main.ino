//#include <TroykaLedMatrix.h>
//#include <PID_dragster.h>
//#include <Octoliner.h>
//#include <Dragster.h>
#define Matrix TroykaLedMatrix


//	###########  Functions  ###########
void stopped();
void waiting();
void running();

void ProcessUserCommand(String cmd);
bool ReadUserCommand(String& cmd, bool wait = false);
void UpdateStateCommands();

void PrintHelp();



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
	static const String StateNames[5];

	static State GetState() { return m_State; }
	static void SetState(State state) { m_State = state; Log::Println("State: " + StateNames[m_State]); }

private:
	static State m_State;
};
static ProgramState::State ProgramState::m_State = ProgramState::WAITING;
static const String ProgramState::StateNames[] = { "STOPPED", "WAITING", "RUNNING" };

//	----- Command
struct Command
{
	const String name;
	const String helptext;
	void (*func)();
	Command(String _name, void (*_func)()) : name(_name), func(_func), helptext("") {}
	Command(String _name, String _help, void (*_func)()) : name(_name), func(_func), helptext(_help) {}
};


//	###########  Variables  ###########
Command programCommands[] = {
	Command(ProgramState::StateNames[0], stopped),
	Command(ProgramState::StateNames[1], waiting),
	Command(ProgramState::StateNames[2], running),
	Command("", nullptr)
};

Command userCommands[] = {
	Command("help", "prints this help message", PrintHelp),
	Command("stop", "sets state to STOP", []() { ProgramState::SetState(ProgramState::STOPPED); }),
	Command("wait", "sets state to WAIT", []() { ProgramState::SetState(ProgramState::WAITING); }),
	Command("run", "sets state to RUN", []() { ProgramState::SetState(ProgramState::RUNNING); }),
	Command("", nullptr)
};



//	###########  Arduino mains  ###########
void setup()
{
	Log::Begin();
	Log::Println("Program begins");
}

void loop()
{
	while (true)
	{
		String userCmd;
		if (ReadUserCommand(userCmd))
			ProcessUserCommand(userCmd);
		UpdateStateCommands();
	}
}


//	###########  Function's Implementations  ###########
void stopped() {}
void waiting() {}
void running() {}

void ProcessUserCommand(String cmd)
{
	Command *command = &userCommands[0];
	while (command->func)
	{
		if (cmd.equals(command->name))
		{
			command->func();
			return;
		}
		command++;
	}
	Log::Println("Undefined command \"" + cmd + "\"");
}

bool ReadUserCommand(String& cmd, bool wait)
{
	if (wait)
		while (Serial.available() <= 0);
	if (Serial.available() > 0)
	{
		cmd = Serial.readStringUntil('\n');
		return true;
	}
	return false;
}


void UpdateStateCommands()
{
	const String& stateName = ProgramState::StateNames[ProgramState::GetState()];
	Command *command = &programCommands[0];
	while (command->func)
	{
		if (stateName.equals(command->name))
		{
			command->func();
			return;
		}
		command++;
	}
	Log::Println("Undefined state name/command \"" + stateName + "\"");
}


void PrintHelp()
{
	Log::Println("Help:");
	for (int i = 0; userCommands[i].func; i++)
		Serial.println("  " + userCommands[i].name + " - " + userCommands[i].helptext);
}

























//

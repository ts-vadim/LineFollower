
#include <Arduino.h>

#include <PID_dragster.h>
#include <Dragster.h>
#include <Octoliner.h>
#include <TroykaLedMatrix.h>
typedef TroykaLedMatrix Matrix;

#include "Log.h"
#include "ProgramState.h"
#include "Command.h"
#include "DragsterUtils.h"


//  ###########  Globals  ###########
float dragsterSpeed = 0.7;
float KP = 0.6;
float KD = 0.02;
float KI = 0.0;

TroykaLedMatrix matrix;
Dragster robot;
Octoliner octoliner;
PID pid(KP, KI, KD);



//	###########  Functions  ###########
void stopped(Command&);
void waiting(Command&);
void running(Command&);
void running2(Command&);

void PrintHelp(Command&);


//	###########  Externs  ###########
extern void ProcessUserCommand(const char* cmd, Command* cmds);
extern bool ReadUserInput(char* cmd, int cmdlen);
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
	Command("halt", "stops program and waits for user input", [](Command&) { while (!ReadUserInput(nullptr, 0)); }),
	Command("reset", "resets arduino", [](Command&) { void (*reset)(void) = 0; reset(); }),
	Command("battery", "prints battery charge", [](Command&) { Log::Print("Battery charge: "); Log::Print(String(GetBatteryCharge() * 100)); Log::Println("%"); }),
	Command("setsens", "(empty)", [](Command&)
		{
			char cmd[4];
			Log::Println("Enter num");
			while (!ReadUserInput(cmd, 4));
			int num = String(cmd).toInt();
			octoliner.setSensitivity(num);
			Log::Println("Sensitivity: " + String(num));
		}),
	Command("", nullptr)
};

static Command programCommands[] = {
	Command(ProgramState::StateNames[0], "stopped", stopped),
	Command(ProgramState::StateNames[1], "waiting", waiting),
	Command(ProgramState::StateNames[2], "running", running2),
	Command("", nullptr)
};


//	##############################################################
//	##################      Arduino mains      ###################
//	##############################################################
void setup()
{
	robot.begin();
	octoliner.begin(120);
	matrix.begin();

	Log::Begin();
	Log::Println("Program begins");

	FindCommand("help", userCommands)->Execute();

	char userCmd[11];
	while (true)
	{
		if (ReadUserInput(userCmd, 11))
			ProcessUserCommand(userCmd, userCommands);
		UpdateStateCommands(ProgramState::StateNames[ProgramState::GetState()], programCommands);
	}
}

void loop()
{
}


//	###########  Function's Implementations  ###########
void stopped(Command&)
{
	static byte diagram[8] = {0};
	static int lineData[8] = {0};

	for (int i = 0; i < 8; i++) {
		lineData[i] = octoliner.analogRead(i);
		diagram[i] = matrix.map(lineData[i], 0, 4095);
	}

	matrix.drawBitmap(diagram);
	matrix.update();
}

void waiting(Command&)
{
	static bool ledState = false;
	static const float cooldown = 0.1;
	static int prev = 0;

	int current = millis();

	if ((current - prev) / 1000.0 >= cooldown)
	{
		prev = current;
		digitalWrite(13, ledState);
		ledState = !ledState;
	}
}

void running(Command&)
{
	static int lineData[8] = {0};
	static int threshold = 2000;
	static float error = 0;

	for (int i = 0; i < 8; i++)
		lineData[i] = octoliner.analogRead(i);

	error = octoliner.mapLine(lineData);
	double output = pid.compute(error);
	robot.driveF(dragsterSpeed - output, dragsterSpeed + output);

	matrix.clear();
	for (float i = 0; i < dragsterSpeed - output; i += 1.0 / 8.0)
		matrix.drawPixel(8 - i * 8.0, 0);
	for (float i = 0; i < dragsterSpeed + output; i += 1.0 / 8.0)
		matrix.drawPixel(8 - i * 8.0, 7);
	matrix.update();
}


void running2(Command&)
{
	static float leftMotor = 0;
	static float rightMotor = 0;
	static int lineData[8] = {0};

	for (int i = 0; i < 8; i++)
		lineData[i] = octoliner.analogRead(i);

	leftMotor = lineData[0] / 4096.0;
	rightMotor = lineData[7] / 4096.0;

	robot.driveF(leftMotor, rightMotor);

	matrix.clear();
	for (float i = 0; i < leftMotor; i += 1.0 / 8.0)
		matrix.drawPixel(9 - i * 8.0, 0);
	for (float i = 0; i < rightMotor; i += 1.0 / 8.0)
		matrix.drawPixel(9 - i * 8.0, 7);
	matrix.update();
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

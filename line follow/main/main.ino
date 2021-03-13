
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
#include "graphics.h"


//  ###########  Globals  ###########
float dragsterSpeed = 0.7;
float KP = 0.3;
float KD = 0.02;
float KI = 0.0;

float drive_proportions[] = { -0.65, -0.6, -0.55, -0.5, 0.5, 0.55, 0.6, 0.65 };

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
extern void FindTryExec(const char* name, Command* commands);


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
	Command("battery", "prints battery charge", [](Command&)
		{
			Log::Print("Battery charge: ");
			Log::Print(String(GetBatteryCharge() * 100));
			Log::Print("%");
			Log::Print(" V: ");
			Log::Println(String(GetBatteryVoltage()));
		}
	),
	Command("sens", "(empty)", [](Command&)
		{
			char cmd[4];
			Log::Println("Enter num");
			while (!ReadUserInput(cmd, 4));
			int num = String(cmd).toInt();
			octoliner.setSensitivity(num);
			Log::Println("Sensitivity: " + String(num));
		}),
	Command("kp", "sets the KP", [](Command&)
		{
			char cmd[10];
			Log::Println("Enter float");
			while (!ReadUserInput(cmd, 10));
			float num = String(cmd).toFloat();
			KP = num;
			Log::Println("KP: " + String(KP));
		}
	),
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
	robot.begin(SWAP_BOTH);
	robot.setMotorLimits(255, 0, 0);
	octoliner.begin(240);
	matrix.begin();

	// --- X
	// |
	// Y
	matrix.setRotation(ROTATION_270);
	Log::Begin();

	FindTryExec("run", userCommands);

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
	DrawOctolinerData(matrix, octoliner);
	matrix.update();

	robot.driveF(0, 0);
}

void waiting(Command&)
{
	static bool ledState = false;
	static const float cooldown = 0.3;
	static int prev = 0;

	int current = millis();

	if ((current - prev) / 1000.0 >= cooldown)
	{
		prev = current;
		digitalWrite(13, ledState);

		float charge = GetBatteryCharge() * 100.0;
		if (ledState)
			if (charge < 10)
				matrix.clear();
			else
				DrawBatteryChargeScreen(matrix, charge);
		else
			DrawBatteryChargeScreen(matrix, 0);

		ledState = !ledState;
	}

	robot.driveF(0, 0);
	matrix.update();
}

void running(Command&)
{
	DrawOctolinerData(matrix, octoliner);
	matrix.update();

	// Average
	float final_proportion = 0.5;
	int values_count = 0;
	for (int i = 0; i < 8; i++)
	{
		float sensor_value = 1.0 - octoliner.analogRead(i) / 4096.0;

		if (sensor_value < 0.5)
		{
			if (values_count == 0)
				final_proportion = drive_proportions[i];
			else
				final_proportion += drive_proportions[i];
			values_count++;
		}
	}

	if (values_count > 0)
		final_proportion /= values_count;

	float left = (final_proportion < 0.0) ? abs(final_proportion) : (final_proportion > 0) ? (1.0 - final_proportion) : 0.5;
	float right = (final_proportion < 0.0) ? 1.0 + final_proportion : (final_proportion > 0.0) ? (final_proportion) : 0.5;
	left *= KP;
	right *= KP;
	robot.driveF(left, right);

	Log::Println("P: " + String(final_proportion) + " Count: " + String(values_count));
	Log::Println("L: " + String(left) + " R: " + String(right));
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

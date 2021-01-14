#include <Arduino.h>


float GetBatteryCharge()
{
	int adcValue = analogRead(A1);
	float V = adcValue * 5.0 / 1023.0;
	float Vbat = V * (46000) / 36000;
	return (Vbat - 3.2) / 0.8;
}
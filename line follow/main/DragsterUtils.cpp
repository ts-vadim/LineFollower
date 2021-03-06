#include "DragsterUtils.h"

float GetBatteryCharge()
{
    static const float R1 = 10000;
    static const float R2 = 36000;
    static const float BATTERY_MIN_V = 3.2;
    static const float BATTERY_MAX_V = 4.2;

	int adcValue = analogRead(A1);
    float V = adcValue * 5.0 / 1023.0;
    float Vbat = V * (R1 + R2) / R2;

    float p = Vbat - BATTERY_MIN_V;

    //Serial.println("V: " + String(V) + " Vbat: " + String(Vbat) + " P: " + String(p));

	return (adcValue / 1024.0 < 0.05 || p < 0) ? 0 : p;
}


float GetBatteryVoltage()
{
    static const float R1 = 10000;
    static const float R2 = 36000;
    static const float BATTERY_MIN_V = 3.2;
    static const float BATTERY_MAX_V = 4.2;

	int adcValue = analogRead(A1);
    float V = adcValue * 5.0 / 1023.0;
    float Vbat = V * (R1 + R2) / R2;

    return Vbat;
}
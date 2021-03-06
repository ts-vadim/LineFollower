#include <Arduino.h>
#include <Octoliner.h>
#include <Dragster.h>
#include <TroykaLedMatrix.h>
#include "DragsterUtils.h"


void DrawOctolinerData(TroykaLedMatrix& matrix, Octoliner& octoliner);

void DrawBatteryChargeScreen(TroykaLedMatrix& matrix, float charge);

void setPixelOnBitmap(byte* bitmap, int x, int y, bool enable);
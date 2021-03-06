#include "graphics.h"


void DrawOctolinerData(TroykaLedMatrix& matrix, Octoliner& octoliner)
{
	byte diagram[8] = {0};
	for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8 - octoliner.analogRead(x) / 512; y++)
		    setPixelOnBitmap(diagram, 7 - x, 7 - y, 1);
	matrix.drawBitmap(diagram);
}


void DrawBatteryChargeScreen(TroykaLedMatrix& matrix, float charge)
{
    static byte battery_img[] = {
        0b00011000,
        0b00111100,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00111100,
    };

    // Clear img
    for (int i = 0; i < 5; i++)
    {
        setPixelOnBitmap(battery_img, 3, 2 + i, 0);
        setPixelOnBitmap(battery_img, 4, 2 + i, 0);
    }

    for (int i = 0; i < charge / 20.0; i++)
    {
        setPixelOnBitmap(battery_img, 3, 6 - i, 1);
        setPixelOnBitmap(battery_img, 4, 6 - i, 1);
    }

    matrix.drawBitmap(battery_img);
}


void setPixelOnBitmap(byte* bitmap, int x, int y, bool enable)
{
    byte mask = 0b10000000 >> x;
    if (enable)
        bitmap[y] |= mask;
    else
        bitmap[y] &= ~mask;
}
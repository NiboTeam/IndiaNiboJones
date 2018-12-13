/*
 ============================================================================
 Name        : showdistance.h
 Author      : Philip Mitzlaff
 Version     : 1.0
 Description : function that shows the distance visual on the display
 ============================================================================
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/gfx.h>
#include <stdio.h>
#include <nibo/delay.h>

/**
 * function that writes lines to the display dependent on the acutal distance
 * @param s1 distance sensor with index 0
 * @param s2 distance sensor with index 1
 * @param s3 distance sensor with index 2
 * @param s4 distance sensor with index 3
 * @param s5 distance sensor with index 4
 */
void showDistance(int s1, int s2, int s3, int s4, int s5) {
	int pos;

	//--- begin drawing outline of Nibo --------------------------

	gfx_move(45, 0);

	gfx_hline(36);
	gfx_vline(15);

	int x = gfx_get_x();
	int y = gfx_get_y();
	gfx_lineTo(x - 10, y + 10);

	x = gfx_get_x();
	y = gfx_get_y();
	gfx_lineTo(x - 16, y);

	x = gfx_get_x();
	y = gfx_get_y();
	gfx_lineTo(x - 10, y - 10);

	x = gfx_get_x();
	y = gfx_get_y();
	gfx_lineTo(x, y - 15);

	//--- end drawing outline of Nibo ----------------------------


	//--- begin of calculate actual position of lines dependent --
	//--- on the given distance of the specific sensor -----------

	pos = (float) 39 / 256 * s1;
	gfx_move(pos, 0);
	gfx_vline(15);

	pos = 32 - ((float) 32 / 256 * s2);
	gfx_move(40 - pos, 19 + pos);
	gfx_lineTo(50 - pos, 29 + pos);

	pos = (float) 32 / 256 * s3;
	gfx_move(56, 63 - pos);
	gfx_hline(15);

	pos = 32 - ((float) 32 / 256 * s4);
	gfx_move(86 + pos, 19 + pos);
	gfx_lineTo(76 + pos, 29 + pos);

	pos = (float) 40 / 256 * s5;
	gfx_move(127 - pos, 0);
	gfx_vline(15);

	//--- end of calculate actual position of lines dependent ----
	//--- on the given distance of the specific sensor -----------

}

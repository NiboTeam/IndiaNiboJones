/*
 * showdistance.c
 * function that shows the distance visual on the screen
 *
 *  Created on: 13.11.2018
 *      Author: philipmitzlaff
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/gfx.h>
#include <stdio.h>
#include <nibo/delay.h>

void showDistance(int s1, int s2, int s3, int s4, int s5) {
	int pos;

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

	pos = (float) 39 / 256 * s1;
	gfx_move(pos, 0);
	gfx_vline(15);

	pos = 32 - ((float) 32 / 256 * s2);
	//45 14
	gfx_move(40 - pos, 19 + pos);

	//55 24
	gfx_lineTo(50 - pos, 29 + pos);

	pos = (float) 32 / 256 * s3;
	gfx_move(56, 63 - pos);
	gfx_hline(15);

	pos = 32 - ((float) 32 / 256 * s4);
	gfx_move(86 + pos, 19 + pos);
	//71 24
	gfx_lineTo(76 + pos, 29 + pos);

	pos = (float) 40 / 256 * s5;
	gfx_move(127 - pos, 0);
	gfx_vline(15);

}

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>
#include <nibo/display.h>
#include <nibo/gfx.h>
#include <stdio.h>
#include <nibo/delay.h>

#include "battery_empty_1.xbm"
#include "battery_empty_2.xbm"
#include "battery_empty_3.xbm"
#include "battery_full.xbm"
#define DEBUG 1

void batteryStatus() {

	double volt = 0;
	char output[20] = "";

	bot_update();
	volt = 0.0166 * bot_supply - 1.19;

#if DEBUG
	sprintf(output, "%2.1f", volt);
	gfx_move(55, 15);
	gfx_print_text(output);
	delay(6);
#endif

	if (volt > 10.2) {
		gfx_move(54, 3);
		gfx_draw_xbm_P(battery_full_width, battery_full_height,
				(PGM_P) battery_full_bits);
	} else if ((9.2 < volt) && (volt < 10)) {
		gfx_move(54, 3);
		gfx_draw_xbm_P(battery_empty_1_width, battery_empty_1_height,
				(PGM_P) battery_empty_1_bits);
	} else if ((8.6 < volt) && (volt < 9.2)) {
		gfx_move(54, 3);
		gfx_draw_xbm_P(battery_empty_2_width, battery_empty_2_height,
				(PGM_P) battery_empty_2_bits);
	} else if ((7 < volt) && (volt < 8.6)) {
		gfx_move(54, 3);
		gfx_draw_xbm_P(battery_empty_3_width, battery_empty_3_height,
				(PGM_P) battery_empty_3_bits);
	}

}


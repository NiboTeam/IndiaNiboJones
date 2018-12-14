/*
 ============================================================================
 Name        : n2switchS3.c
 Author      : Daniel Wittekind
 Version     : 1.0
 Description : Funktionen zur Statusabfrage des Schalters S3 auf dem Nibo
 ============================================================================
 */

#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/delay.h>


/*
 * Gibt 1 zurueck, falls der Schalter S3 gedrueckt wurde
 */
int s3_was_pressed() {

	// Rueckgabe
	int result = 0;

	/*
	 * Diese Funktion arbeitet naeher an der Hardware, da fuer
	 * den Schalter keine fertige Bibliothek existiert
	 *
	 * Ueber die Funktion get_input_bit kann der Zustand (1 oder 0)
	 * eines Pins des Controllers abgefragt werden. Der Schalter S3
	 * ist durch die Header-Datei iodefs.h als IO_INPUT_1 definiert.
	 *
	 * Wenn der Schalter gedrueckt ist, wird 0 zurueckgeliefert
	 */
	// Schalter ist gedrueckt
	if (!get_input_bit(IO_INPUT_1)) {
		/*
		 * Bei mechanischen Schaltern kommt es vor, dass unmittelbar
		 * nach dem Druecken, der Pegel noch schnell zwischen 1 und 0
		 * schwankt. Das nennt man Prellen oder englisch bounce.
		 * Daher wird der Eingang entprellt (debounce), indem kurz
		 * gewartet wird und erneut ueberprueft wird, ob der Pegel noch
		 * der Erwartung entspricht
		 */
		delay(15);

		// Wenn der Schalter noch gedrueckt ist...
		while (!get_input_bit(IO_INPUT_1)) {
			/*
			 * ... nichts tun.
			 * Die Schleife wird verlassen, wenn der Schalter sich wieder laesst
			 */
		}
		// Schalter losgelassen
		// zwecks Entprellen warten
		delay(15);

		// Wert bestaetigen
		if (get_input_bit(IO_INPUT_1))
			// Der Schalter wurde jetzt ordnungsgemae\3 gedrueckt, daher positive Rueckgabe
			result = 1;
	} else {
		// Der Schalter wurde nicht gedrueckt
		result = 0;
	}

	// Rueckgabe
	return result;
}

/**
 * Gibt 1 zurueck, falls der Schalter S3 gerade gedrueckt wird
 */
int s3_is_pressed() {

	int result = 0;

	// s3 pressed down
	if (!get_input_bit(IO_INPUT_1)) {
		// debounce 20ms
		delay(20);
		if (!get_input_bit(IO_INPUT_1)) {
			result = 1;
		}
	} else {
		result = 0;
	}

	return result;
}

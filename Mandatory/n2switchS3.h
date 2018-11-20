/*
 ============================================================================
 Name        : n2switchS3.h
 Author      : Daniel Wittekind
 Version     : 1.0
 Description : Funktionen zur Statusabfrage des Schalters S3 auf dem Nibo
 ============================================================================
 */
#ifndef SWITCHS3_H_
#define SWITCHS3_H_

/**
 * returns 1, if switch S3 was pressed down and released
 */
int s3_was_pressed();

/**
 * returns 1, if switch S3 is currently pressed down
 */
int s3_is_pressed();

#endif /* SWITCHS3_H_ */

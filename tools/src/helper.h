
/******************************************************************************
 * helper.h
 * written by carstene1ns, 2014
 *****************************************************************************/

#ifndef _helper_h_
#define _helper_h_

unsigned char *loadfile1(char *filename);
unsigned char *loadfile2(char *filename, unsigned long *filesize);

void changefileext(char *szPath, char *ext);

#endif /* _helper_h_ */

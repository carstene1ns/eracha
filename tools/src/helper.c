
/******************************************************************************
 * helper.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

/* loads a whole file to a buffer */
unsigned char *loadfile1(char *filename)
{
  unsigned long unused;

  return loadfile2(filename, &unused);
}

/* loads a whole file to a buffer, gives back size */
unsigned char *loadfile2(char *filename, unsigned long *filesize)
{
  FILE *filepointer;
  unsigned char *filebuffer;
  size_t bytesread;

  /* open the file */
  filepointer = fopen(filename, "rb");
  if(filepointer == NULL)
  {
    perror("Could not open file, Error");
    return NULL;
  }

  /* get size of the file */
  fseek(filepointer, 0L, SEEK_END);
  *filesize = ftell(filepointer);
  rewind(filepointer);

  /* allocate a block of memory big enough for the file */
  filebuffer = (unsigned char *)malloc(sizeof(unsigned char) * *filesize);
  if(filebuffer == NULL)
  {
    perror("Could not allocate memory, Error");
    return NULL;
  }

  /* read whole file in buffer */
  bytesread = fread(filebuffer, 1, *filesize, filepointer);
  if(bytesread != *filesize)
  {
    perror("Could not read file, Error");
    return NULL;
  }

  return filebuffer;
}

void changefileext(char *szPath, char *ext)
{
  /* find file name */
  char *pFile = strrchr(szPath, '/');
  if(pFile == NULL)
    pFile = szPath;
  else
    pFile = pFile + 1;

  /* change extension */
  char *pExt = strrchr(pFile, '.');
  if(pExt != NULL)
    strcpy(pExt, ext);
  else
    strcat(pFile, ext);
}

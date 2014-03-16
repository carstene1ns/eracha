
/******************************************************************************
 * sme2ascii.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

/* main function */
int main(int argc, char *argv[])
{
  unsigned int width, height, tile, offset, i, j;
  unsigned char *buf;

  if(argc == 1)
  {
    printf("usage: sme2ascii file.sme\n");
    exit(1);
  }

  char *filename = argv[1];

  buf = loadfile1(filename);
  if(buf == NULL)
    exit(1);

  /* parse file header */
  offset = 33;
  width = buf[offset] | (buf[offset + 1] << 8);
  height = buf[offset + 4] | (buf[offset + 5] << 8);

  printf("'%s' level size is %dx%d.\n", filename, width, height);

  offset = 41;
  for(i = 0; i < height; i++)
  {
    for(j = 0; j < width; j = j + 2)
    {
      tile = buf[offset + i * width * 2 + j] | (buf[offset + i * width * 2 + j + 1] << 8);

      if(tile != 0)
        printf("%03x", tile);
      else
        printf("   ");
    }
    printf("\n");
  }

  /* all good, exit */
  free(buf);
  exit(0);
}

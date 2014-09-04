
/******************************************************************************
 * pal2gpl.c - converts a game palette to a GIMP Palette (.gpl)
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

/* main function */
int main(int argc, char *argv[])
{
  char *filename_from, *filename_to;
  unsigned int count;
  unsigned long size;
  unsigned char *buf;

  /* print usage info, if no file given */
  if(argc == 1)
  {
    printf("pal2gpl\n");
    printf("----------------------------------\n");
    printf("usage: pal2gpl file.pal [file.gpl]\n");
    exit(0);
  }
  else if(argc == 3)
  {
    /* both filenames given */
    filename_from = argv[1];
    filename_to = argv[2];
  }
  else if(argc == 2)
  {
    /* only one filename given */
    filename_from = argv[1];
    filename_to = (char *)malloc(sizeof(filename_from) + 1);
    if(filename_to == NULL)
    {
      perror("malloc");
      exit(1);
    }
    strcpy(filename_to, filename_from);
    changefileext(filename_to, ".gpl");
  }
  else
  {
    printf("Too many arguments!\n");
    exit(1);
  }

  buf = loadfile2(filename_from, &size);
  if(buf == NULL)
    exit(1);

  /* 3 bytes = 1 color */
  count = size / 3;
  printf("'%s' has %d colors, file size is %li.\n", filename_from, count, size);

  FILE *des = fopen(filename_to, "wb");
  if(des == NULL)
  {
    perror("create file");
    exit(1);
  }

  /* header */
  fprintf(des, "GIMP Palette\nName: eracha\n#\n");

  unsigned int i, r, g, b;
  for(i = 0; i < size; i = i + 3)
  {
    r = (buf[i] << 2) | (buf[i] >> 4);
    g = (buf[i + 1] << 2) | (buf[i + 1] >> 4);
    b = (buf[i + 2] << 2) | (buf[i + 2] >> 4);

    /* write color */
    fprintf(des, "%3d %3d %3d\n", r, g, b);
  }

  /* all good, close file and exit */
  fclose(des);
  free(buf);
  exit(0);
}

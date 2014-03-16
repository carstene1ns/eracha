
/******************************************************************************
 * san2ascii.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

/* main function */
int main(int argc, char *argv[])
{
  unsigned int count, offset, width, height, bytes, ignore_flag, pos_in_row,
    padding;
  unsigned long size;
  unsigned char *buf;

  if(argc == 1)
  {
    printf("usage: san2ascii file.san\n");
    exit(1);
  }

  char *filename = argv[1];

  buf = loadfile2(filename, &size);
  if(buf == NULL)
    exit(1);

  /* parse file header */
  count = buf[10];
  printf("'%s' has %d images, data size is %li bytes.\n", filename, count,
    size-12);

  offset = 12;
  for(unsigned int id = 0; id < count; id++)
  {
    /* parse image header */
    width = buf[offset];
    height = buf[offset + 2];
    bytes = buf[offset + 4] | (buf[offset + 5] << 8);

    printf("image %d, %dx%d, size is %d.\n", id + 1, width, height, bytes);

    pos_in_row = 0;
    ignore_flag = 0;
    for(unsigned int i = 0; i < bytes; i++)
    {
      if(ignore_flag)
      {
        ignore_flag = 0;
        continue;
      }

      /* check for rle padding */
      if(buf[offset + 6 + i] == '\0')
      {
        /* ignore next byte in outer loop */
        ignore_flag = 1;
        padding = buf[offset + 6 + i + 1];

        for(unsigned int j = 0; j < padding; j++)
        {
          printf("   ");
          pos_in_row++;
          if(pos_in_row == width)
          {
            printf("|\n");
            pos_in_row = 0;
          }
        }
      }
      else
      {
        printf("%2x ", buf[offset + 6 + i]);
        pos_in_row++;
        if(pos_in_row == width)
        {
          printf("I\n");
          pos_in_row = 0;
        }
      }
    }
    for(unsigned int i = 0; i < width - pos_in_row; i++)
    {
      printf("   ");
    }
    printf("!\n\n");

    /* set offset for next image */
    offset += bytes + 8;
  }

  /* all good, exit */
  free(buf);
  exit(0);
}

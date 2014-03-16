
/******************************************************************************
 * pal2png.c - converts a game palette to a png image
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#include "helper.h"

/* main function */
int main(int argc, char *argv[])
{
  char *filename_from, *filename_to;
  unsigned int count;
  unsigned long size;
  unsigned char *buf;
  unsigned int palette[256];
  FILE *des;

  png_structp png_ptr;
  png_infop info_ptr;

  /* print usage info, if no file given */
  if(argc == 1)
  {
    printf("pal2png\n");
    printf("----------------------------------\n");
    printf("usage: pal2png file.pal [file.png]\n");
    exit(0);
  }
  else if(argc == 3)
  {
    /* filenames given */
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
    changefileext(filename_to, ".png");
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
  printf("'%s' has %d colors, this is %d rows. file size is %li.\n",
    filename_from, count, count / 256, size);

  des = fopen(filename_to, "wb");
  if(des == NULL)
  {
    perror("create file");
    exit(1);
  }

  /* png init */
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(png_ptr == NULL)
  {
    fclose(des);
    exit(1);
  }
  info_ptr = png_create_info_struct(png_ptr);
  if(info_ptr == NULL)
  {
    fclose(des);
    png_destroy_write_struct(&png_ptr, NULL);
    exit(1);
  }
  /* error handling */
  if(setjmp(png_jmpbuf(png_ptr)))
  {
    fclose(des);
    png_destroy_write_struct(&png_ptr, NULL);
    exit(1);
  }

  /* set output to file */
  png_init_io(png_ptr, des);

  /* set header */
  png_set_IHDR(png_ptr, info_ptr, 256, count / 256, 8,
    PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  /* write header */
  png_write_info(png_ptr, info_ptr);

  unsigned int red, green, blue;
  for(unsigned int i = 0; i < count / 256; i++)
  {
    for(unsigned int j = 0; j < 256; j++)
    {
      red =  buf[i * 768 + j * 3];
      green = buf[i * 768 + j * 3 + 1];
      blue = buf[i * 768 + j * 3 + 2];

      /* shift vga palette */
      palette[j] = (0xFF << 24) |                     /* a */
                   ((blue << 2 | blue >> 4) << 16) |  /* b */
                   ((green << 2 | green >> 4) << 8) | /* g */
                   (red << 2 | red >> 4);             /* r */

      //printf("pal=%d, pos=%d, r=%3d g=%3d b=%3d\n", i, j, red, green, blue);
    }
    /* write row */
    png_write_row(png_ptr, (png_bytep)palette);
  }

  /* finish */
  png_write_end(png_ptr, info_ptr);

  /* all good, clean up, close files and exit */
  png_destroy_write_struct(&png_ptr, NULL);
  fclose(des);
  free(buf);
  exit(0);
}


/******************************************************************************
 * til2png.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#include "helper.h"

#define ENDOFHEADER 0x17
#define ROWTILES 40

/* main function */
int main(int argc, char *argv[])
{
  char *filename_from, *filename_to;
  unsigned int count;
  unsigned long size;
  unsigned char *buf, *row_buf;

  png_structp png_ptr;
  png_infop info_ptr;
  png_colorp palette;
  png_byte idx = 1;

  /* print usage info, if no file given */
  if(argc == 1)
  {
    printf("til2png\n");
    printf("----------------------------------\n");
    printf("usage: til2png file.til [file.png]\n");
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
    filename_to = (char *)malloc(sizeof(char) * strlen(filename_from) + 1);
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

  unsigned int id, tile_size, i, j, k;

  /* parse file header */
  tile_size = buf[ENDOFHEADER - 3];
  count = buf[ENDOFHEADER - 2] | (buf[ENDOFHEADER - 1] << 8);

  printf("'%s' has %d tiles, with tile size %d, file size is %li.\n",
    filename_from, count, tile_size, size-23);

  FILE *des = fopen(filename_to, "wb");
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
  png_set_IHDR(png_ptr, info_ptr, tile_size * ROWTILES,
    count * tile_size / ROWTILES, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  /* set palette */
  palette = (png_colorp)png_malloc(png_ptr, 256 * sizeof(png_color));
  unsigned int palpos;
  for(i = 0; i < 256; i++)
  {
    palpos = ENDOFHEADER + tile_size * tile_size * count + i * 3;
    palette[i].red   = (buf[palpos] << 2) | (buf[palpos] >> 4);
    palette[i].green = (buf[palpos + 1] << 2) | (buf[palpos + 1] >> 4);
    palette[i].blue  = (buf[palpos + 2] << 2) | (buf[palpos + 2] >> 4);
    //printf("r %d g %d b %d\n", palette[i].red, palette[i].green, palette[i].blue);
  }
  png_set_PLTE(png_ptr, info_ptr, palette, 256);

  /* add transparency */
  png_set_tRNS(png_ptr, info_ptr, &idx, 1, NULL);

  /* write header */
  png_write_info(png_ptr, info_ptr);

  row_buf = (unsigned char *)malloc(sizeof(unsigned char) * tile_size * ROWTILES);
  if(row_buf == NULL)
  {
    perror("malloc");
    exit(1);
  }

  /* copy tiles */
  for(id = 0; id < count; id = id + ROWTILES)
  {
    /* tile height */
    for(i = 0; i < tile_size; i++)
    {
      /* tile count */
      for(j = 0; j < ROWTILES; j++)
      {
        /* tile width */
        for(k = 0; k < tile_size; k++)
        {
          /* construct row */
          row_buf[j * tile_size + k] = buf[ENDOFHEADER + (id+j) * tile_size * tile_size + i * tile_size + k];
        }
      }
      /* write row */
      png_write_row(png_ptr, (png_bytep)row_buf);
    }
  }

  /* finish */
  png_write_end(png_ptr, info_ptr);

  /* all good, clean up, close files and exit */
  png_destroy_write_struct(&png_ptr, NULL);
  free(row_buf);
  free(buf);
  exit(0);
}


/******************************************************************************
 * san2png.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

/* the default palettes */
#include "palettes/open_pal.h"
#include "palettes/era0XX_pal.h"

/* main function */
int main (int argc, char *argv[])
{
  unsigned int count;
  unsigned long size;
  unsigned char *buf;
  unsigned char *pixels;
  size_t res;

  png_structp png_ptr;
  png_infop info_ptr;
  png_colorp palette;
  png_byte idx = 1;

  if(argc == 1)
  {
    printf("usage: san2png file.san");
    exit(1);
  }

  char *filename = argv[1];

  FILE *fp = fopen(filename,"rb");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  buf = (unsigned char *)malloc(sizeof(unsigned char) * size);
  if(buf == NULL)
  {
    perror("malloc");
    exit(1);
  }

  res = fread(buf, 1, size, fp);
  if(res != size)
  {
    perror("fread");
    exit(1);
  }
  fclose(fp);

  /* parse file header */
  count = buf[10];
  printf("'%s' has %d images, file size is %li.\n", filename, count, size-12);

  unsigned int id, offset, width, height, bytes, i, j, ignore_flag, pos_in_row, padding;
  offset = 12;

  for (id = 0; id < count; id++)
  {
    /* FIXME */
    char filename_new[10];
    sprintf(filename_new, "%s_%02d.png", "temp", id);

    FILE *des = fopen(filename_new, "wb");
    if(des == NULL)
    {
      perror("fopen");
      exit(1);
    }

    /* png init */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
      fclose(des);
      exit(1);
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
      fclose(des);
      png_destroy_write_struct(&png_ptr, NULL);
      exit(1);
    }
    /* error handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
      fclose(des);
      png_destroy_write_struct(&png_ptr, NULL);
      exit(1);
    }
    /* set output to file */
    png_init_io(png_ptr, des);

    /* parse image header */
    width = buf[offset] | (buf[offset + 1] << 8);
    height = buf[offset + 2] | (buf[offset + 3] << 8);
    bytes = buf[offset + 4] | (buf[offset + 5] << 8);

    printf("image %d, %dx%d, size is %d.\n", id + 1, width, height, bytes);

    /* set header */
    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
      PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    palette = (png_colorp)png_malloc(png_ptr, 256 * sizeof(png_color));
    for (i = 0; i < 256; i++)
    {
      /* FIXME */
#if 0
      palette[i].red   = (open_pal[i * 3] << 2) | (open_pal[i * 3] >> 4);
      palette[i].green = (open_pal[i * 3 + 1] << 2) | (open_pal[i * 3 + 1] >> 4);
      palette[i].blue  = (open_pal[i * 3 + 2] << 2) | (open_pal[i * 3 + 2] >> 4);
#else
      palette[i].red   = (era0XX_pal[i * 3] << 2) | (era0XX_pal[i * 3] >> 4);
      palette[i].green = (era0XX_pal[i * 3 + 1] << 2) | (era0XX_pal[i * 3 + 1] >> 4);
      palette[i].blue  = (era0XX_pal[i * 3 + 2] << 2) | (era0XX_pal[i * 3 + 2] >> 4);
#endif
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);

    /* transparency */
    png_set_tRNS(png_ptr, info_ptr, &idx, 1, NULL);

    /* write header */
    png_write_info(png_ptr, info_ptr);

    /* a line of pixels */
    pixels = (unsigned char *)malloc(sizeof(unsigned char) * width);
    if(pixels == NULL)
    {
      perror("malloc");
      exit(1);
    }

    pos_in_row = 0;
    ignore_flag = 0;
    for (i = 0; i < bytes; i++)
    {
      if (ignore_flag)
      {
        ignore_flag = 0;
        continue;
      }

      /* check for rle padding */
      if (buf[offset + 6 + i] == '\0')
      {
        /* ignore next byte in outer loop */
        ignore_flag = 1;
        padding = buf[offset + 6 + i + 1];

        for (j = 0; j < padding; j++)
        {
          pixels[pos_in_row] = 0;
          pos_in_row++;
          if (pos_in_row == width)
          {
            /* write out the line */
            png_write_row(png_ptr, (png_bytep)pixels);
            pos_in_row = 0;
          }
        }
      }
      else
      {
        pixels[pos_in_row] = buf[offset + 6 + i];
        pos_in_row++;
        if (pos_in_row == width)
        {
          /* write out the line */
          png_write_row(png_ptr, (png_bytep)pixels);
          pos_in_row = 0;
        }
      }
    }
    if(pos_in_row < width)
    {
      for (j = 0; j < width - pos_in_row; j++)
      {
        pixels[pos_in_row] = 0;
        pos_in_row++;
      }
      /* write out the last line */
      png_write_row(png_ptr, (png_bytep)pixels);
    }

    /* finish */
    png_write_end(png_ptr, info_ptr);

    /* cleanup */
    free(pixels);
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(des);

    /* set offset for next image */
    offset += bytes + 8;
  }

  /* all good, exit */
  free(buf);
  exit(0);
}


/******************************************************************************
 * san2ascii.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* main function */
int main (int argc, char *argv[]) {
  unsigned int count;
  unsigned long size;
  unsigned char *buf;
  size_t res;

  char *filename = argv[1];

  FILE *fp = fopen(filename,"rb");
  if(fp == NULL) {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  buf = (unsigned char *)malloc(sizeof(char) * size);
  if(buf == NULL) {
    perror("malloc");
    exit(1);
  }

  res = fread(buf, 1, size, fp);
  if(res != size) {
    perror("fread");
    exit(1);
  }

  /* parse file header */
  count = buf[10];
  printf("'%s' has %d images, file size is %li.\n",
    filename, count, size-12);

  unsigned int id, offset, width, height, bytes, i, j, ignore_flag, pos_in_row, padding;
  offset = 12;
  for (id = 0; id < count; id++) {
    /* parse image header */
    width = buf[offset];
    height = buf[offset + 2];
    bytes = buf[offset + 4] | (buf[offset + 5] << 8);

    printf("image %d, %dx%d, size is %d.\n",
      id + 1, width, height, bytes);

    pos_in_row = 0;
    ignore_flag = 0;
    for (i = 0; i < bytes; i++) {
      if (ignore_flag) {
        ignore_flag = 0;
        continue;
      }

      /* check for rle padding */
      if (buf[offset + 6 + i] == '\0') {
        /* ignore next byte in outer loop */
        ignore_flag = 1;
        padding = buf[offset + 6 + i + 1];

        for (j = 0; j < padding; j++) {
          printf("   ");
          pos_in_row++;
          if (pos_in_row == width) {
            printf("|\n");
            pos_in_row = 0;
          }
        }
      } else {
        printf("%2x ", buf[offset + 6 + i]);
        pos_in_row++;
        if (pos_in_row == width) {
          printf("I\n");
          pos_in_row = 0;
        }
      }
    }
    for (j = 0; j < width - pos_in_row; j++) {
      printf("   ");
    }
    printf("!\n\n");

    /* set offset for next image */
    offset += bytes + 8;
  }

  /* all good, exit */
  fclose(fp);
  free(buf);
  exit(0);
}

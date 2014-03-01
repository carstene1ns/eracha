
/******************************************************************************
 * sme2ascii.c
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* main function */
int main (int argc, char *argv[]) {
  unsigned int width, height;
  unsigned char tile[2];
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

  unsigned int offset, i, j;

  /* parse file header */
  offset = 33;
  width = buf[offset] | (buf[offset + 1] << 8);
  height = buf[offset + 4];

  printf("'%s' level size is %dx%d, file size is %li.\n",
    filename, width, height, size-41);

  offset = 41;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j = j + 2) {
      tile[0] = buf[offset + i * width * 2 + j];
      tile[1] = buf[offset + i * width * 2 + j + 1];

      if (tile[0] == '\0' && tile[1] == '\0') {
        //printf("    ");
        printf("  ");
      } else {
        //printf("%02x%02x", tile[0], tile[1]);
        printf("%02x", tile[0]);
      }
    }
    printf("\n");
  }

  /* all good, exit */
  fclose(fp);
  free(buf);
  exit(0);
}

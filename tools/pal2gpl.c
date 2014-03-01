
/******************************************************************************
 * pal2gpl.c - converts a game palette to a GIMP Palette (GPL)
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* helper functions */
void changefileext(char *szPath, char *ext) {
  /* find file name */
  char *pFile = strrchr(szPath, '/');
  pFile = pFile == NULL ? szPath : pFile+1;

  /* change extension */
  char *pExt = strrchr(pFile, '.');
  if (pExt != NULL)
    strcpy(pExt, ext);
  else
    strcat(pFile, ext);
}

/* main function */
int main (int argc, char *argv[]) {
  char *filename_from, *filename_to;
  unsigned int count;
  unsigned long size;
  unsigned char *buf;
  size_t res;

  /* print usage info, if no file given */
  if (argc == 1) {
    printf("pal2gpl\n");
    printf("-------------------------------\n");
    printf("usage: pal2gpl file.pal [file.gpl]\n");
    exit(0);
  } else if(argc == 3) {
    /* filenames given */
    filename_from = argv[1];
    filename_to = argv[2];
  } else if(argc == 2) {
    /* only one filename given */
    filename_from = argv[1];
    filename_to = (char *)malloc(sizeof(filename_from)+1);
    if (filename_to == NULL) {
      perror("malloc");
      exit(1);
    }
    strcpy(filename_to, filename_from);
    changefileext(filename_to, ".gpl");
  } else {
    printf("Too many arguments!\n");
    exit(1);
  }

  FILE *fp = fopen(filename_from, "rb");
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

  /* 3 bytes = 1 color */
  count = size / 3;
  printf("'%s' has %d colors, file size is %li.\n",
    filename_from, count, size);

  FILE *des = fopen(filename_to, "wb");
  if(!des) {
    perror("create file");
    exit(1);
  }

  /* header */
  fprintf(des, "GIMP Palette\nName: eracha\n#\n");

  unsigned int i;
  for (i = 0; i < size; i = i + 3) {
    /* write color:            -> R,           G,               B */
    fprintf(des, "%3d %3d %3d\n", buf[i] << 2, buf[i + 1] << 2, buf[i + 2] << 2);
  }

  /* all good, close files and exit */
  fclose(des);
  fclose(fp);
  free(buf);
  exit(0);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "helpers.h"
#include "game.h"
#include "file.h"

char *FilenameFromId(int id) {
  char *temp = NULL;
  temp = malloc(16);
  if (temp == NULL) {
    perror("malloc");
    exit(1);
  }

  /* hardcode the folder name for now */
  strcpy(temp, "data/");

  switch(id) {
    case ARCHIVE_MAPS:
      strcat(temp, "MIRM.MMF");
      break;
    case ARCHIVE_BKGS:
      strcat(temp, "MIRB.MBF");
      break;
    case ARCHIVE_MUS1:
      strcat(temp, "MIR10.MMF");
      break;
  }
  return temp;
}

char *LoadLevel(int lvl, unsigned int width, unsigned int height) {
  UNUSED(lvl);
  UNUSED(width);
  UNUSED(height);

  return NULL;
}

char *LoadBackgroundF(int bkg, unsigned *size) {
  printf("Loading Background #%d\n", bkg);

  char *temp_buf = UnpackArchive(ARCHIVE_BKGS, bkg - LEVEL_01 + 1, size);

  /* error while loading */
  if(*size == 0)
    return NULL;

  *size = (*size + PCX_HEADER_SIZE - BCK_HEADER_SIZE);
  /* add PCX header and clear it */
  unsigned char *buffer = malloc(*size * sizeof(unsigned char));
  if (buffer == NULL) {
    perror("malloc");
    exit(1);
  }
  memset(buffer, 0, PCX_HEADER_SIZE);

  /* set some values of the pcx header, rest can be calculated */
  /* thanks to http://www.fileformat.info/format/pcx/corion.htm */
  buffer[0] = 10;                     /* id */
  buffer[1] = 5;                      /* version */
  buffer[2] = 1;                      /* rle */
  buffer[3] = 8;                      /* bits per pixel */
  buffer[8] = (PCX_WIDTH-1) & 0xFF;   /* width low bits */
  buffer[9] = (PCX_WIDTH-1) >> 8;     /* width high bits */
  buffer[10] = (PCX_HEIGHT-1) & 0xFF; /* height low bits */
  buffer[11] = (PCX_HEIGHT-1) >> 8;   /* height high bits */
  buffer[65] = 1;                     /* number of color planes */
  buffer[66] = PCX_WIDTH & 0xFF;      /* bytes per scanline */
  buffer[67] = PCX_WIDTH >> 8;        /* bytes per scanline */

  /* add the raw data */
  memcpy(&buffer[PCX_HEADER_SIZE], &temp_buf[BCK_HEADER_SIZE], *size - PCX_HEADER_SIZE);
  free(temp_buf);

  return (char *)buffer;
}

char *LoadMusicF(int mod, unsigned *size) {
  printf("Loading Music #%d\n", mod);

  int archive, entry;
  if(mod >= LEVEL_01 && mod < LEVEL_COUNT) {
    archive = ARCHIVE_MUS1;
    entry = mod - LEVEL_01 + 1;
  } else if(mod >= BOSS_01 && mod < BOSS_COUNT) {
    archive = ARCHIVE_MUS2;
    entry = mod - BOSS_01 + 1;
  } else {
    archive = ARCHIVE_MUS3;
    entry = mod - MUSIC_MENU + 1;
  }

  char *buffer = UnpackArchive(archive, entry, size);

  /* error while loading */
  if(size == 0)
    return NULL;

  return buffer;
}

/* Archive */

char *UnpackArchive(int file, int number, unsigned *size) {
  /* open the specified file in binary mode */
  char *filename = FilenameFromId(file);
  FILE *fp = fopen(filename, "rb");
  if(fp == NULL) {
    perror("fopen");
    exit(1);
  }

  /* get file count */
  int numfiles = 0;
  fseek(fp, 9, SEEK_SET);
  int r = fread(&numfiles, 1, 1, fp);
  assert(r == 1);

  /* get file sizes and offsets */
  unsigned int loffset = ARCHIVE_HEADER_SIZE;
  unsigned int lsize = 0;
  for (int i = 0; i < number; i++) {
    /* read 3 bytes */
    fread(&lsize, 1, 3, fp);
    /* skip 1 (0x00 padding) */
    fseek(fp, 1, SEEK_CUR);
    loffset += lsize;
  }
  loffset -= lsize;

  /* get file names, stored at end of file */
  char lname[13];
  fseek(fp, -ARCHIVE_FOOTER_SIZE, SEEK_END);
  for (int i = 0; i < number; i++) {
    /* file name in dos format (XXXXXXXX.XXX)
     * as they are seperated by 0x00, we get string terminators for free
     */
    fread(&lname, 1, 13, fp);
    /* convert filenames to lowercase */
    for (int j = 0; j < 13; j++) {
      lname[j] = tolower(lname[j]);
    }
  }

  char *buffer = malloc(lsize * sizeof(char));
  if(buffer == NULL) {
    perror("malloc");
    exit(1);
  } 

  printf("loaded %s (%u bytes)...\n", lname, lsize);

  fseek(fp, loffset, SEEK_SET);
  unsigned int asize = fread(buffer, 1, lsize, fp);

  if(asize != lsize) {
    printf("requested %u bytes, got %u bytes.", lsize, asize);
  }
  *size = asize;

  /* all good, exit */
  fclose(fp);
  free(filename);

  return buffer;
}

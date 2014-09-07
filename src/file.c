
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "game.h"
#include "file.h"

char *FilenameFromId(int id) {
  char *temp = NULL;
  temp = malloc(16);
  if(temp == NULL) {
    perror("malloc");
    exit(1);
  }

  /* hardcode the folder name for now */
  strcpy(temp, "data/");

  switch(id) {
    case ARCHIVE_BKGS:
      strcat(temp, "MIRB.MBF");
      break;
    case ARCHIVE_TILE:
      strcat(temp, "MIRT.MTF");
      break;
    case ARCHIVE_MAPS:
      strcat(temp, "MIRM.MMF");
      break;
    case ARCHIVE_ATTR:
      strcat(temp, "MIRA.MAF");
      break;
    case ARCHIVE_MUS1:
      strcat(temp, "MIR10.MMF");
      break;
    case ARCHIVE_MUS2:
      strcat(temp, "MIR11.MMF");
      break;
    case ARCHIVE_MUS3:
      strcat(temp, "MIR12.MMF");
      break;
    case ARCHIVE_SND1:
      strcat(temp, "MIR01.MWF");
      break;
    case ARCHIVE_SND2:
      strcat(temp, "MIR02.MWF");
      break;
    case ARCHIVE_SND3:
      strcat(temp, "MIR03.MWF");
      break;
    default:
      strcat(temp, "no.file");
      break;
  }
  return temp;
}

unsigned char *LoadLevelF(int lvl, unsigned int *width, unsigned int *height) {
  unsigned int size;
  unsigned char *temp_buf = (unsigned char *)UnpackArchive(ARCHIVE_MAPS,
                            lvl - LEVEL_01 + 1, &size);

  /* error while loading */
  if(size == 0)
    return NULL;

  /* parse file header */
  *width = temp_buf[SME_HEADER_SIZE - 8] | (temp_buf[SME_HEADER_SIZE - 7] << 8);
  *height = temp_buf[SME_HEADER_SIZE - 4] | (temp_buf[SME_HEADER_SIZE - 3] << 8);

  printf("level width: %u, height: %u\n", *width, *height);

  /* cache level in memory */
  unsigned char *buf = malloc((size - SME_HEADER_SIZE) * sizeof(unsigned char));
  if(buf == NULL) {
    perror("malloc");
    /* cleanup */
    free(temp_buf);
    *width = 0;
    *height = 0;
    return NULL;
  }
  memcpy(buf, &temp_buf[SME_HEADER_SIZE], size - SME_HEADER_SIZE);

  /* cleanup */
  free(temp_buf);

  return buf;
}

unsigned char *LoadAttributesF(int lvl, unsigned int *size) {
  unsigned int temp_size;
  unsigned char *temp_buf = (unsigned char *)UnpackArchive(ARCHIVE_ATTR,
                            lvl - LEVEL_01 + 1, &temp_size);

  /* error while loading */
  if(temp_size == 0L)
    return NULL;

  *size = temp_size - ATR_HEADER_SIZE;

  /* cache attributes in memory */
  unsigned char *buf = malloc(*size * sizeof(unsigned char));
  if(buf == NULL) {
    perror("malloc");
    /* cleanup */
    free(temp_buf);
    *size = 0;
    return NULL;
  }
  memcpy(buf, &temp_buf[ATR_HEADER_SIZE], *size);

  /* cleanup */
  free(temp_buf);

  return buf;
}

char *LoadTilesetF(int tset, unsigned int *size) {
  printf("Loading Tileset #%d\n", tset - LEVEL_01 + 1);

  unsigned int temp_size;
  char *temp_buf = UnpackArchive(ARCHIVE_TILE, tset - LEVEL_01 + 1, &temp_size);

  /* error while loading */
  if(temp_size == 0)
    return NULL;

  *size = temp_size - TIL_HEADER_SIZE;
  char *buffer = malloc(*size * sizeof(char));
  if(buffer == NULL) {
    perror("malloc");
    exit(1);
  }

  /* copy raw data */
  memcpy(buffer, &temp_buf[TIL_HEADER_SIZE], *size);
  free(temp_buf);

  return (char *)buffer;
}

char *LoadBackgroundF(int bkg, unsigned int *size) {
  printf("Loading Background #%d\n", bkg - LEVEL_01 + 1);

  char *temp_buf = UnpackArchive(ARCHIVE_BKGS, bkg - LEVEL_01 + 1, size);

  /* error while loading */
  if(*size == 0)
    return NULL;

  *size = *size + PCX_HEADER_SIZE - BCK_HEADER_SIZE;
  /* add PCX header and clear it */
  unsigned char *buffer = malloc(*size * sizeof(unsigned char));
  if(buffer == NULL) {
    perror("malloc");
    exit(1);
  }
  memset(buffer, 0, PCX_HEADER_SIZE);

  /* set some values of the pcx header, rest can be calculated */
  /* thanks to http://www.fileformat.info/format/pcx/corion.htm */
  buffer[0] = 10;                       /* id */
  buffer[1] = 5;                        /* version */
  buffer[2] = 1;                        /* rle */
  buffer[3] = 8;                        /* bits per pixel */
  buffer[8] = (PCX_WIDTH - 1) & 0xFF;   /* width low bits */
  buffer[9] = (PCX_WIDTH - 1) >> 8;     /* width high bits */
  buffer[10] = (PCX_HEIGHT - 1) & 0xFF; /* height low bits */
  buffer[11] = (PCX_HEIGHT - 1) >> 8;   /* height high bits */
  buffer[65] = 1;                       /* number of color planes */
  buffer[66] = PCX_WIDTH & 0xFF;        /* bytes per scanline */
  buffer[67] = PCX_WIDTH >> 8;          /* bytes per scanline */

  /* add the raw data */
  memcpy(&buffer[PCX_HEADER_SIZE], &temp_buf[BCK_HEADER_SIZE], *size - PCX_HEADER_SIZE);

  /* cleanup */
  free(temp_buf);

  return (char *)buffer;
}

char *LoadMusicF(int mod, unsigned int *size) {
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

  printf("Loading Music %d/%d\n", archive, entry);

  char *buffer = UnpackArchive(archive, entry, size);

  /* error while loading */
  if(*size == 0)
    return NULL;

  return buffer;
}

/* Archive */

char *UnpackArchive(int file, int number, unsigned int *size) {
  /* open the specified file in binary mode */
  char *filename = FilenameFromId(file);
  FILE *fp = fopen(filename, "rb");

  /* error */
  if(fp == NULL) {
    perror("fopen");
    /* cleanup */
    *size = 0;
    return NULL;
  }

  /* get file count */
  int numfiles = 0;
  fseek(fp, 9, SEEK_SET);
  int r = fread(&numfiles, 1, 1, fp);

  /* error */
  if(r != 1 || numfiles < 1 || numfiles > 10) {
    printf("Error: Number of files in archive is wrong!");
    /* cleanup */
    *size = 0;
    return NULL;
  }

  /* get file sizes and offsets */
  unsigned int loffset = ARCHIVE_HEADER_SIZE;
  unsigned int lsize = 0;
  for(int i = 0; i < number; i++) {
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
  for(int i = 0; i < number; i++) {
    /* file name in dos format (XXXXXXXX.XXX)
     * as they are seperated by 0x00, we get string terminators for free
     */
    fread(&lname, 1, 13, fp);
    /* convert filenames to lowercase */
    for(int j = 0; j < 13; j++) {
      lname[j] = tolower(lname[j]);
    }
  }

  char *buffer = malloc(lsize * sizeof(char));
  if(buffer == NULL) {
    perror("malloc");
    exit(1);
  }

  printf("loading %u bytes from %s...\n", lsize, lname);

  fseek(fp, loffset, SEEK_SET);
  unsigned int asize = fread(buffer, 1, lsize, fp);

  /* error */
  if(asize != lsize) {
    printf("Error: requested %u bytes, got %u bytes!", lsize, asize);
    /* cleanup */
    *size = 0;
    free(buffer);
    return NULL;
  }
  *size = asize;

  /* all good, exit */
  fclose(fp);
  free(filename);

  return buffer;
}


/******************************************************************************
 * edc.c ERACHA data converter
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BCKHEADER_SIZE 19  /* 0x [SaemSong]PCXtoHNP 0x */
#define PCXHEADER_SIZE 128 /* version, size, empty 16 color palette, padding, ... */
/* erachas pcx images have fixed size */
#define PCX_WIDTH 320
#define PCX_HEIGHT 200

/* helper functions */
char *changefileext(char *szPath, char *ext) {
  /* find file name */
  char *pFile = strrchr(szPath, '/');
  pFile = pFile == NULL ? szPath : pFile+1;

  /* change extension */
  char *pExt = strrchr(pFile, '.');
  if (pExt != NULL)
    strcpy(pExt, ext);
  else
    strcat(pFile, ext);

  return pFile;
}

/* main function */
int main (int argc, char *argv[]) {
  char *filename_from, *filename_to;
  int mode;
  unsigned long size;
  char *buf;
  size_t res;

  /* print usage info, if no file given */
  if (argc == 1) {
    printf("ERACHA data converter\n");
    printf("-------------------------------\n");
    printf("usage: edc file1.ext1 [file2.ext2]\n\n");
    printf("modes supported:\n");
    printf(" - BCK => PCX\n");
    exit(0);
  } else if(argc == 3) {
    /* filenames given */
    filename_from = argv[1];
    filename_to = argv[2];
  } else if(argc == 2) {
    /* only one filenames given */
    filename_from = argv[1];
    filename_to = (char *)malloc(sizeof(filename_from)+1);
    if (filename_to == NULL) {
      perror("malloc");
      exit(1);
    }
  } else {
    printf("Too many arguments!\n");
    exit(1);
  }

  /* check file extension */
  char *ext = strrchr(filename_from, '.');
  if (!ext) {
    /* no extension */
    printf("filename '%s' has no extension!\n", filename_from);
    exit(1);
  } else if (strncmp(ext, ".bck", 4) == 0 || strncmp(ext, ".BCK", 4) == 0) {
    mode = 1;
    printf("Converting bck to pcx\n");
/*  } else if (strncmp(ext, ".pcx", 4) == 0 || strncmp(ext, ".PCX", 4) == 0) {
    mode = 2;
    printf("converting pcx to bck\n");
*/
  } else {
    printf("extension is '%s', do not know what to do...(yet!)\n", ext + 1);
    exit(1);
  }

  /* open the specified file in binary mode */
  FILE *fp = fopen(filename_from,"rb");
  if(fp == NULL) {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  buf = (char *)malloc(sizeof(char) * size);
  if(buf == NULL) {
    perror("malloc");
    exit(1);
  }

  res = fread(buf, 1, size, fp);
  if(res != size) {
    perror("fread");
    exit(1);
  }

  /* check mode */
  if (mode == 1) {
    /* check header */
    if (buf[11] == 'P' && buf[12] == 'C' && buf[13] == 'X') {
      /* check for empty file name to copy to */
      if (filename_to[0] == '\0') {
        filename_to = changefileext(filename_from, ".pcx");
      }

      printf("'%s' has size of %li bytes, setting resolution to %dx%d.\n",
      filename_from, size-BCKHEADER_SIZE, PCX_WIDTH, PCX_HEIGHT);
      printf("Writing %li bytes to %s\n", size-BCKHEADER_SIZE+PCXHEADER_SIZE, filename_to);

      FILE *des = fopen(filename_to, "wb");
      if(!des) {
        perror("create file");
        exit(1);
      }
   
      unsigned char *pcxheader = malloc(PCXHEADER_SIZE);
      if (pcxheader == NULL) {
        perror("malloc");
        exit(1);
      }

      /* set some values of the pcx header, rest can be calculated */
      /* thanks to http://www.fileformat.info/format/pcx/corion.htm */
      pcxheader[0] = 10;                     /* id */
      pcxheader[1] = 5;                      /* version */
      pcxheader[2] = 1;                      /* rle */
      pcxheader[3] = 8;                      /* bits per pixel */
      pcxheader[8] = (PCX_WIDTH-1) & 0xFF;   /* width low bits */
      pcxheader[9] = (PCX_WIDTH-1) >> 8;     /* width high bits */
      pcxheader[10] = (PCX_HEIGHT-1) & 0xFF; /* height low bits */
      pcxheader[11] = (PCX_HEIGHT-1) >> 8;   /* height high bits */
      pcxheader[65] = 1;                     /* number of color planes */
      pcxheader[66] = PCX_WIDTH & 0xFF;      /* bytes per scanline */
      pcxheader[67] = PCX_WIDTH >> 8;        /* bytes per scanline */

      /* write out our pcx header and the raw data */
      fwrite(pcxheader, PCXHEADER_SIZE, 1, des);
      fwrite(&buf[BCKHEADER_SIZE], size-BCKHEADER_SIZE, 1, des);

      free(pcxheader);
      fclose(des);
    } else {
      printf("'%s' is no valid BCK file!\n", filename_from);
    }
  } else {
    printf("-UNKNOWN MODE-\n");
  }

  /* all good, exit */
  fclose(fp);
  free(buf);
  exit(0);
}

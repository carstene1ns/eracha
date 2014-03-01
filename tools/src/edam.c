
/******************************************************************************
 * edam.c ERACHA data archive manipulator
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* magic numbers */
#define END_OF_HEADER 1034
#define BEGIN_OF_FOOTER 3328

/* This holds info about a file in the archive */
typedef struct {
  char name[13];       /* file name 8+3 */
  unsigned int offset; /* position in archive file */
  unsigned int size;   /* size in bytes */
} afile;

/* extract a given file  */
void extractfile(FILE *src, char *filename, unsigned int offset, unsigned int size) {
  printf("Copying %i bytes from %x to %s\n", size, offset, filename);

  FILE *des = fopen(filename, "wb");
  if(!des) {
    perror("create file");
    exit(1);
  }
 
  char *buf = malloc(size);
  if (buf == NULL) {
    perror("malloc");
    exit(1);
  } 

  fseek(src, offset, SEEK_SET);
  fread(buf, size, 1, src);
  fwrite(buf, size, 1, des);

  free(buf);
  fclose(des);
}

/* main function */
int main (int argc, char *argv[]) {
  int numfiles = 0;
  afile *files;
  char *filename;

  /* print usage info, if no file given */
  if (argc == 1) {
    printf("ERACHA data archive manipulator\n");
    printf("-------------------------------\n");
    printf("usage: edam [options] ARCHIVE.m[a|b|m|t|w]f [FILES]\n\n");
    printf("valid options are:\n");
    printf(" -i : shows information about ARCHIVE (default)\n");
    printf(" -e : extracts ARCHIVE to current directory\n");
    printf("(-a : adds FILES to ARCHIVE) TODO\n");
    exit(0);
  } else if(argc == 3) {
    /* mode given */
    filename = argv[2];
  } else {
    /* no mode given */
    filename = argv[1];
  }
  
  /* open the specified file in binary mode */
  FILE *fp = fopen(filename,"rb");
  if(fp == NULL) {
    perror("fopen");
    exit(1);
  }

  /* get file count */
  fseek(fp, 9, SEEK_SET);
  fread(&numfiles, 1, 1, fp);
  printf("INFO: Found %d files in %s.\n", numfiles, filename);

  if (numfiles > 0) {
    int i;
    files = (afile *) malloc(sizeof(afile) * numfiles);
    if (files == NULL) {
      perror("malloc");
      exit(1);
    }

    /* get file sizes and offsets */
    unsigned int offset = END_OF_HEADER;
    for (i = 0; i < numfiles; i++) {
      /* read 3 bytes */
      fread(&files[i].size, 1, 3, fp);
      /* skip 1 (0x00 padding) */
      fseek(fp, 1, SEEK_CUR);
      files[i].offset = offset;
      offset += files[i].size;
    }

    /* get file names, stored at end of file */
    fseek(fp, -BEGIN_OF_FOOTER, SEEK_END);
    for (i = 0; i < numfiles; i++) {
      /* file name in dos format (XXXXXXXX.XXX)
       * as they are seperated by 0x00, we get string terminators for free
       */
      fread(files[i].name, 1, 13, fp);
    }
  }

  /* check for mode parameter */
  if (argc == 3 && (strncmp(argv[1], "-e", 2) == 0)) {
    /* extract all files */
    int i;
    for (i = 0; i < numfiles; i++) {
      extractfile(fp, files[i].name, files[i].offset, files[i].size);
    }
  } else {
    /* default: display info */
    int i;
    for (i = 0; i < numfiles; i++) {
      printf("INFO: File %d @ offset 0x%06x: name: '%s', size: %d bytes\n",
        i+1, files[i].offset, files[i].name, files[i].size);
    }
  }

  /* all good, exit */
  free(files);
  fclose(fp);
  exit(0);
}

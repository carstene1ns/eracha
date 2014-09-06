
#ifndef FILE_H
#define FILE_H

/* magic numbers */
#define ARCHIVE_HEADER_SIZE 1034
#define ARCHIVE_FOOTER_SIZE 3328
#define BCK_HEADER_SIZE 19
#define PCX_HEADER_SIZE 128
#define SME_HEADER_SIZE 41
#define ATR_HEADER_SIZE 4
#define TIL_HEADER_SIZE 23
#define PCX_WIDTH 320
#define PCX_HEIGHT 200

char *FilenameFromId(int id);

unsigned char *LoadLevelF(int lvl, unsigned int *width, unsigned int *height);
unsigned char *LoadAttributesF(int lvl, unsigned int *size);
char *LoadTilesetF(int tset, unsigned int *size);
char *LoadBackgroundF(int bkg, unsigned int *size);
char *LoadMusicF(int mod, unsigned int *size);

char *UnpackArchive(int file, int number, unsigned int *size);

#endif /* FILE_H */

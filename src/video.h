
#ifndef VIDEO_H
#define VIDEO_H

#define TILE_SIZE 8
#define TILE_BLOCK (TILE_SIZE * TILE_SIZE)
#define TILE_ROWS 75
#define TILE_COLS 40
#define TILE_COUNT 3000
#define PALETTE_COLORS 256
#define PALETTE_SIZE (PALETTE_COLORS * 3)
#define BACKGROUND_SHIFT 0.8

/* TODO: make dynamic */
#define STATIC_SCROLL 4

int InitVideo();
void DeinitVideo();

int LoadBackground(int pic);
int LoadTileset(int set);

void DrawLevel();

#endif /* VIDEO_H */

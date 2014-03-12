TIL Format
==========

Tile data: all floors, background objects and other stuff, saved in blocks with
palette at the end.
The tiles can be arranged to 40 in a row, output is then a 320x600 pixels image.

HEADER
------

 * offset 0x0 - 0x12: file magic "*SaemSong Tile Data*"
 * offset 0x13: tile size (always 8, so a tile is 8 x 8 = 64 pixels)
 * offset 0x14 - 0x15: tile count (always 3000)

FOOTER
------

  * 768 bytes palette with 256 colors rgb in 6 bit vga format, every value
    needs to be shifted to 8 bit rgb format like this:
    ```rgb = (vga << 2) | (vga >> 4)```

---

Additional Information
----------------------

Not all palette colors are used by the tiles, the others are used by the game
objects in the levels and are the same for every til file.

```til2png``` can convert tile data to a png image.

A thumbnail of the converted tiles is available here:

![TIL2PNG](imgs/eracha_til2png.png)

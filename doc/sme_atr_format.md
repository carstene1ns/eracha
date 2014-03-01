SME & ATR Format (Level files)
==============================

They are clearly related, because the atr files all have a corresponding sme
file, which is twice the size.

The sme file contains the blocks with a link to the tileset and the atr
file the attributes (like clipping).

SME Header
----------

0x0 - 0x13: file magic "0x7 [SaemSong] MapData 0x0"

0x14 - 0x20: name of tileset with padding to the end (0x0), e.g. "era001.TIL"

0x21 - 0x22: count of objects

0x23 - 0x24: padding (0x0)

0x25: count of objects in row

array with 2 bytes of blocks in level related to the linked tileset.

ATR Header
----------

0x0 - 0x1: count of objects

0x2: count of objects in row

array with 4 bytes of attributes follows until EOF.


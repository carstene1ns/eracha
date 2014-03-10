SME & ATR Format (Level files)
==============================

They are clearly related, because the atr files all have a corresponding sme
file, which is twice the size.

The sme file contains the blocks with a link to the tileset and the atr
file the attributes (like clipping, object positions).

SME Header
----------

0x0 - 0x13: file magic "0x7 [SaemSong] MapData 0x0"

0x14 - 0x20: name of tileset with padding to the end (0x0), e.g. "era001.TIL"

0x21 - 0x22: count of blocks in width

0x23 - 0x24: padding (0x0)

0x25 - 0x26: count of blocks in height

DATA
----

array with 2 bytes of blocks in level, interpreted as 16 bit integer with tile
number of linked tileset

ATR Header
----------

0x0 - 0x1: count of blocks in width

0x2 - 0x3: count of blocks in height

DATA
----

array with 4 bytes of attributes, interpreted as 32 bit integer with a
combination of flags in 2^X or 0 (means no interaction, just the background
is drawn there)

known flags (X):

0:
1: has transparency
2: solid floor
3: draw in foreground (partly hides player sprite)
4: solid walls and ceiling
5: left edge (stagger animation)
6: right edge (stagger animation)
7:
8: something about stairs/slopes?
9:
10:
11:
12:
13:
14:
15:
16:
17:
18:
19:
20:
21:
22:
23:
24:
25:
26:
27:
28:
29:
30:
31:

Additional Information
----------------------

```sme2ascii``` can show an ascii presentation of the levels.
```levelviewer``` displays a level

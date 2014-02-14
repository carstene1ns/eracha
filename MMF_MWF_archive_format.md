Multi Mod/Map/Wave Format (MMF/MWF)
===================================

This files describes the archive format used by the following files:

- MIR01.MWF (wave sounds)
- MIR02.MWF
- MIR03.MWF

- MIR10.MMF (mod music)
- MIR11.MMF
- MIR12.MMF

- MIRM.MMF (maps)

HEADER
------

length: 0x40A (1034) bytes

offset 0x0 - 0x8: padding (0x00)

offset 0x9: number of files

following offsets: information about the files, likely lengths or offsets, todo.

padding till end of header (0x00)

DATA
----

the files, raw, not seperated

FOOTER
------

length: 0xD00 (3328) bytes

list of filenames, seperated by 3 or 4 bytes of 0x00

padding till end of file (0x00)

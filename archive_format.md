Multi/Mirinae archive Format
============================

This files describes the archive format used by the following files.
The file extension contains information about the file types inside
the archive:

- MIR01.MWF ([W] - wave sounds)
  MIR02.MWF
  MIR03.MWF
- MIR10.MMF ([M] - mod music)
  MIR11.MMF
  MIR12.MMF
- MIRA.MAF ([A] - attributes)
- MIRB.MBF ([B] - backgrounds)
- MIRM.MMF ([M] - maps)
- MIRT.MTF ([T] - tiles)

HEADER
------

length: 0x40A (1034) bytes

offset 0x0 - 0x8: padding (0x00)

offset 0x9: number of files

following offsets: lengths of files

padding till end of header (0x00)

DATA
----

the files, raw, not seperated

FOOTER
------

length: 0xD00 (3328) bytes

list of filenames 8+3 dos format, seperated by 0x00

padding till end of file (0x00)

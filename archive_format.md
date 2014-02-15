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

File Contents
-------------

This is the output of edam, from the tools directory:

INFO: Found 10 files in DATA/MIR01.MWF.
INFO: File 1 @ offset 0x00040a: name: 'MRI01.WAV', size: 4466 bytes
INFO: File 2 @ offset 0x00157c: name: 'MRI02.WAV', size: 2389 bytes
INFO: File 3 @ offset 0x001ed1: name: 'MRI03.WAV', size: 1866 bytes
INFO: File 4 @ offset 0x00261b: name: 'MRI04.WAV', size: 2072 bytes
INFO: File 5 @ offset 0x002e33: name: 'MRI05.WAV', size: 3222 bytes
INFO: File 6 @ offset 0x003ac9: name: 'MRI06.WAV', size: 1906 bytes
INFO: File 7 @ offset 0x00423b: name: 'MRI07.WAV', size: 4988 bytes
INFO: File 8 @ offset 0x0055b7: name: 'MRI08.WAV', size: 7724 bytes
INFO: File 9 @ offset 0x0073e3: name: 'MRI09.WAV', size: 1674 bytes
INFO: File 10 @ offset 0x007a6d: name: 'AB01.WAV', size: 9062 bytes

INFO: Found 2 files in DATA/MIR02.MWF.
INFO: File 1 @ offset 0x00040a: name: 'inter!.wav', size: 54114 bytes
INFO: File 2 @ offset 0x00d76c: name: 'inter@.wav', size: 15614 bytes

INFO: Found 5 files in DATA/MIR03.MWF.
INFO: File 1 @ offset 0x00040a: name: 'boss1.wav', size: 38066 bytes
INFO: File 2 @ offset 0x0098bc: name: 'boss2.wav', size: 16114 bytes
INFO: File 3 @ offset 0x00d7ae: name: 'boss3.wav', size: 9384 bytes
INFO: File 4 @ offset 0x00fc56: name: 'boss4.wav', size: 22154 bytes
INFO: File 5 @ offset 0x0152e0: name: 'boss5.wav', size: 18740 bytes

INFO: Found 9 files in DATA/MIR10.MMF.
INFO: File 1 @ offset 0x00040a: name: 'era001.mod', size: 64468 bytes
INFO: File 2 @ offset 0x00ffde: name: 'era002.mod', size: 129610 bytes
INFO: File 3 @ offset 0x02fa28: name: 'era011.mod', size: 116178 bytes
INFO: File 4 @ offset 0x04bffa: name: 'era012.mod', size: 67636 bytes
INFO: File 5 @ offset 0x05c82e: name: 'era021.mod', size: 120468 bytes
INFO: File 6 @ offset 0x079ec2: name: 'era022.mod', size: 79034 bytes
INFO: File 7 @ offset 0x08d37c: name: 'era031.mod', size: 107782 bytes
INFO: File 8 @ offset 0x0a7882: name: 'era032.mod', size: 94264 bytes
INFO: File 9 @ offset 0x0be8ba: name: 'era041.mod', size: 168820 bytes

INFO: Found 5 files in DATA/MIR11.MMF.
INFO: File 1 @ offset 0x00040a: name: 'boss1.mod', size: 114364 bytes
INFO: File 2 @ offset 0x01c2c6: name: 'boss2.mod', size: 126828 bytes
INFO: File 3 @ offset 0x03b232: name: 'boss3.mod', size: 109220 bytes
INFO: File 4 @ offset 0x055cd6: name: 'boss4.mod', size: 124978 bytes
INFO: File 5 @ offset 0x074508: name: '!_!.mod', size: 3132 bytes

INFO: Found 3 files in DATA/MIR12.MMF.
INFO: File 1 @ offset 0x00040a: name: 'ending.mod', size: 80990 bytes
INFO: File 2 @ offset 0x014068: name: 'interf.mod', size: 55434 bytes
INFO: File 3 @ offset 0x0218f2: name: 'map_.mod', size: 92706 bytes

INFO: Found 9 files in DATA/MIRA.MAF.
INFO: File 1 @ offset 0x00040a: name: 'era001.atr', size: 403204 bytes
INFO: File 2 @ offset 0x062b0e: name: 'era002.atr', size: 400004 bytes
INFO: File 3 @ offset 0x0c4592: name: 'era011.atr', size: 400004 bytes
INFO: File 4 @ offset 0x126016: name: 'era012.atr', size: 416004 bytes
INFO: File 5 @ offset 0x18b91a: name: 'era021.atr', size: 412388 bytes
INFO: File 6 @ offset 0x1f03fe: name: 'era022.atr', size: 400004 bytes
INFO: File 7 @ offset 0x251e82: name: 'era031.atr', size: 240004 bytes
INFO: File 8 @ offset 0x28c806: name: 'era032.atr', size: 361604 bytes
INFO: File 9 @ offset 0x2e4c8a: name: 'era041.atr', size: 300004 bytes

INFO: Found 9 files in DATA/MIRB.MBF.
INFO: File 1 @ offset 0x00040a: name: 'ERA001.BCK', size: 53081 bytes
INFO: File 2 @ offset 0x00d363: name: 'ERA002.BCK', size: 71270 bytes
INFO: File 3 @ offset 0x01e9c9: name: 'ERA011.BCK', size: 33361 bytes
INFO: File 4 @ offset 0x026c1a: name: 'ERA012.BCK', size: 25227 bytes
INFO: File 5 @ offset 0x02cea5: name: 'ERA021.BCK', size: 43030 bytes
INFO: File 6 @ offset 0x0376bb: name: 'ERA022.BCK', size: 98706 bytes
INFO: File 7 @ offset 0x04f84d: name: 'ERA031.BCK', size: 52441 bytes
INFO: File 8 @ offset 0x05c526: name: 'ERA032.BCK', size: 38280 bytes
INFO: File 9 @ offset 0x065aae: name: 'ERA041.BCK', size: 74776 bytes

INFO: Found 9 files in DATA/MIRM.MMF.
INFO: File 1 @ offset 0x00040a: name: 'ERA001.SME', size: 201641 bytes
INFO: File 2 @ offset 0x0317b3: name: 'ERA002.SME', size: 200041 bytes
INFO: File 3 @ offset 0x06251c: name: 'ERA011.SME', size: 200041 bytes
INFO: File 4 @ offset 0x093285: name: 'ERA012.SME', size: 208041 bytes
INFO: File 5 @ offset 0x0c5f2e: name: 'ERA021.SME', size: 206233 bytes
INFO: File 6 @ offset 0x0f84c7: name: 'ERA022.SME', size: 200041 bytes
INFO: File 7 @ offset 0x129230: name: 'ERA031.SME', size: 120041 bytes
INFO: File 8 @ offset 0x146719: name: 'ERA032.SME', size: 180841 bytes
INFO: File 9 @ offset 0x172982: name: 'ERA041.SME', size: 150041 bytes

INFO: Found 9 files in DATA/MIRT.MTF.
INFO: File 1 @ offset 0x00040a: name: 'ERA001.TIL', size: 192791 bytes
INFO: File 2 @ offset 0x02f521: name: 'ERA002.TIL', size: 192791 bytes
INFO: File 3 @ offset 0x05e638: name: 'ERA011.TIL', size: 192791 bytes
INFO: File 4 @ offset 0x08d74f: name: 'ERA012.TIL', size: 192791 bytes
INFO: File 5 @ offset 0x0bc866: name: 'ERA021.TIL', size: 192791 bytes
INFO: File 6 @ offset 0x0eb97d: name: 'ERA022.TIL', size: 192791 bytes
INFO: File 7 @ offset 0x11aa94: name: 'ERA031.TIL', size: 192791 bytes
INFO: File 8 @ offset 0x149bab: name: 'ERA032.TIL', size: 192791 bytes
INFO: File 9 @ offset 0x178cc2: name: 'ERA041.TIL', size: 192791 bytes

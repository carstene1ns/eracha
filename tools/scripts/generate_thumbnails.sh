#!/bin/sh

###############################################################################
# generate_thumbnails.sh uses imagemagick to create a thumbnail of data files
# written by carstene1ns, 2014
###############################################################################

# tilesets
montage -label '%f' era*.png -tile 3x -geometry x200+10+10 -border 2 \
  thumb_tilesets.png

# backgrounds
montage -label '%f' *.PCX *.pcx -tile 3x -geometry x100+10+10 -border 2 \
  thumb_backgrounds.png

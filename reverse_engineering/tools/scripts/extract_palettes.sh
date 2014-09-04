#!/bin/sh

###############################################################################
# extract_palettes.sh extracts last 768 bytes from file and generates header
# written by carstene1ns, 2014
###############################################################################

for file in *.til; do
  # remove extension
  newfile=${file%.til}

  # be verbose
  echo "${file} → ${newfile}.pal, ${newfile}_pal.h"

  # extract palette
  tail -c 768 ${file} > ${newfile}.pal

  # generate .h file
  xxd -i ${newfile}.pal > ${newfile}_pal.h
done

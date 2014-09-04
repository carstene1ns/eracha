#!/bin/sh

###############################################################################
# generate_html.sh converts markdown to html for preview
# written by carstene1ns, 2014
###############################################################################

for file in *.md; do
  # remove extension
  newfile=${file%.md}

  # be verbose
  echo "${file} → ${newfile}.html"

  # extract title
  TITLE=$(head -n 1 ${file})

  # insert html head
  echo -e "<!DOCTYPE html>\n<html lang=\"en\">" > ${newfile}.html
  echo -e "<head>\n<meta charset=\"utf-8\">" >> ${newfile}.html
  echo -e "<title>$TITLE</title>\n</head>\n<body>\n<div>" >> ${newfile}.html

  # generate page
  markdown_py -o html5 --noisy -x smart_strong -x fenced_code -x tables \
    ${file} >> ${newfile}.html

  # insert html foot
  echo -e "\n</div>\n</body>\n</html>" >> ${newfile}.html
done

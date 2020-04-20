#!/bin/sh

cd src
cinclude2dot.pl > source.dot
cd ..
dot -Teps src/source.dot | zathura -
rm src/source.dot

#!/bin/sh

cd src
cinclude2dot.pl > source.dot
cd ..
dot -Teps -Kneato src/source.dot | zathura -
rm src/source.dot

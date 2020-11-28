#!/bin/sh

for file in ./test/*; do
	gcc "$file" $@ -I src -o ./testbin/$(echo $file | rev | cut -d "/" -f 1 | rev).out
done

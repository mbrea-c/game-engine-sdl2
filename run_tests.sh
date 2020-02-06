#!/bin/sh

for file in ./testbin/*; do
	$file && echo "\033[32m--> TEST successful:\033[0m $file" || echo "\033[31m!!! TEST failed:\033[0m $file"
done

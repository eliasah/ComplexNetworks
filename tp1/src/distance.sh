#!/bin/bash
echo "This may take a while"

FILES=../../data/*.data
for f in $FILES
do
  echo "Processing $f file..."
  echo "$f" >> dist_results.txt
  ./distance $f 2 >> dist_results.txt
done


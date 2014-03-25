#!/bin/bash
echo "This may take a while"

FILES=../../data/*.data
for f in $FILES
do
  echo "Processing $f file..."
  echo "$f" >> cc_results.txt
  ./compconnexes $f >> cc_results.txt
done


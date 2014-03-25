#!/bin/bash
echo "This may take a while"

FILES=../../data/*.data
for f in $FILES
do
  echo "Processing $f file..."
  ./prep_data $f
done


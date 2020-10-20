#!/bin/bash

./calc
# for smaller screens
#pager -~ r
# for bigger screens
echo ""
cat r | less
echo "Call \"./calc -csv\" for CSV file..."
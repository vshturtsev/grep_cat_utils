#!/bin/bash

ARGS=$@
FILE_STD='cat.cmp'
FILE_MY='my_cat.cmp'
FILE_DIF='diff.cmp'

echo ARGS = $ARGS 
echo files_cmp = $FILE_STD $FILE_MY
echo cat $ARGS > $FILE_STD
cat $ARGS > $FILE_STD

echo ./my_cat $ARGS > $FILE_MY
./my_cat $ARGS > $FILE_MY

echo diff $FILE_STD $FILE_MY
diff $FILE_STD $FILE_MY > $FILE_DIF

if [ -s "$FILE_DIF" ]; then
  echo "Error! There is difference, see file $FILE_DIF"
else
  echo "Ok! No difference."
  rm $FILE_DIF
fi

rm $FILE_STD $FILE_MY

exit 0

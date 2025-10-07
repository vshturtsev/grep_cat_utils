#!/bin/bash

ARGS=$@
FILE_STD='grep.cmp'
FILE_MY='my_grep.cmp'
FILE_DIF='diff.cmp'

echo ARGS = $ARGS
echo files_cmp = $FILE_STD $FILE_MY
echo grep $ARGS ' > $FILE_STD'
grep $ARGS > $FILE_STD #2>&1

echo ./s21_grep $ARGS ' > $FILE_MY'
./s21_grep $ARGS > $FILE_MY #2>&1

echo diff $FILE_STD $FILE_MY
diff $FILE_STD $FILE_MY 
diff $FILE_STD $FILE_MY > $FILE_DIF

if [ -s "$FILE_DIF" ]; then
  echo "Error! There is difference, see file $FILE_DIF"
else
  echo "Ok! No difference."
  rm $FILE_DIF
fi

rm $FILE_STD $FILE_MY

exit 0

echo make clean
make clean
echo build my_cat
make

flags=("" "-b" "-n" "-e" "-E" "-s" "-t" "-T" "-v" "-benstv")

for flag in "${flags[@]}"; do
  echo "--- Start test flag '$flag' ---"
  cat $flag ../../test_files/*.txt > cat.cmp
  ./my_cat $flag ../../test_files/*.txt > my_cat.cmp
  diff cat.cmp my_cat.cmp > temp_diff.cmp
  if [ -s "temp_diff.cmp" ]; then
    cp temp_diff.cmp diff"$flag".cmp
    echo "    Error! There is difference, see file diff"$flag".cmp"
  else
    echo "       Ok! No difference."
  fi
  echo "    End test flag '$flag'"
  echo ""
done

rm temp_diff.cmp cat.cmp my_cat.cmp

# echo test -b
# cat -b files/*.txt > cat.cmp
# ../my_cat -b files/*.txt > s21.cmp
# diff cat.cmp s21.cmp > diff.cmp
# if [ -s "diff.cmp" ]; then
#   echo "Error! There is difference, see file diff.cmp"
# else
#   echo "Ok! No difference."
# fi
# echo end test -b

# echo test -n
# cat -n files/*.txt > cat.cmp
# ../my_cat -n files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -n

# echo test -e
# cat -e files/*.txt > cat.cmp
# ../my_cat -e files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -e

# echo test -E
# cat -E files/*.txt > cat.cmp
# ../my_cat -E files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -E

# echo test -s
# cat -s files/*.txt > cat.cmp
# ../my_cat -s files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -s

# echo test -t
# cat -t files/*.txt > cat.cmp
# ../my_cat -t files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -t

# echo test -T
# cat -T files/*.txt > cat.cmp
# ../my_cat -T files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -T

# echo test -v
# cat -v files/*.txt > cat.cmp
# ../my_cat -v files/*.txt > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -v

# echo test -benstv with non_exist file
# cat -benstv nofile files/*.txt  > cat.cmp
# ../my_cat -benstv nofile files/*.txt  > s21.cmp
# diff cat.cmp s21.cmp
# echo end test -benstv
# : '
# '
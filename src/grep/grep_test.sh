#!/bin/bash
make clean
make

SUCCESS=0
FAIL=0
COUNTER=0
DIFF=""

my_cmd=("./my_grep")
my_log=("my_grep.log")
sys_cmd=("grep")
sys_log=("grep.log")
pattern_file=("../../test_files/grep_patterns")

flags=(-i -v -c -l -n -h -o -s -ino -iv -ins -nh -co -noish -lvis)

files=(
	"grep.c"
	"grep.h"
	"grep.c grep.h"
	"../../test_files/*.txt"
)

run() {
	let "COUNTER++"
	"${my_cmd[@]}" -f "${pattern_file[@]}" $1 $2 > "${my_log[@]}"
	"${sys_cmd[@]}" -f "${pattern_file[@]}" $1 $2 > "${sys_log[@]}"
    #valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ${my_cmd[@]} $1 $2 >> test_my_grep.log 2>&1
	DIFF="$(diff -s "${my_log[@]}" "${sys_log[@]}")"
	result=$?
	
	if [ $result -eq 0 ]
	then 
		let "SUCCESS++"
        echo -e "\e[32m#$COUNTER ::SUCCESS:: CMD: $my_cmd PARAM: $1 FILE: $2\e[0m"
	elif [ $result -eq 1 ]
	then
		let "FAIL++"
		echo -e "\e[31m#$COUNTER ::FAIL:: CMD: $my_cmd PARAM: $1 FILE: $2\e[0m"
	else
		echo "WARNING: wrong with the diff command"
	fi
	rm -f "${my_log[@]}" "${sys_log[@]}"
}

echo "RUN TEST grep"
printf "\n"

for f in "${flags[@]}"
do
	for i in "${files[@]}"
	do
		run "$f" "$i"
	done

done

printf "\n"
echo -e "\e[32mSUCCESS: $SUCCESS\e[0m"
echo -e "\e[31mFAIL: $FAIL\e[0m"
echo "ALL: $COUNTER"

#!/bin/bash

list=("*_RMS.DATA" "*_PCG.DATA")
count=0

echo ${list[@]}

for nn in ${list[@]}; do 

	echo file in: ${nn}
	files[${count}]=${nn}.COMP

	file=${files[${count}]}
	echo file out: $file

	grep COMPDAT ${nn} -A100 | \
	grep COMPDAT -v | grep -e "--" -v | \
	head -n -2 | \
	awk '{printf "%9.5f\n", $8}' \
	> ${files[${count}]}

	# echo out: files[${count}]

	nlines[${count}]=`cat ${file} | wc -l`

	let count=count+1

	# debug:
	# grep COMPDAT ${nn} -A100 | \
	# grep COMPDAT -v | grep -e "--" -v | \
	# head -n -2 > test.out 
	# cat test.out  | wc -l
	# cat test.out | awk '{printf "%9.5f\n", $8}' | wc -l

done

echo array: files ${files[@]}
echo length of array: files ${#files[@]}

echo array: nlines ${nlines[@]}
echo length of array: nlines ${#nlines[@]}

if [ ${nlines[0]} == ${nlines[1]} ] 
	then
	echo "same number of lines => run diff()"

	diff ${files[0]} ${files[1]} -c1
fi




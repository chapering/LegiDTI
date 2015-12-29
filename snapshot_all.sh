#!/bin/bash

for cls in normal_whole normal_allfb normal_partial
do
	for tid in 1 2
	do
		for pos in pos0 pos1
		do
			sh gooverblock_withprompt.sh $cls $tid $pos
		done
	done
done

exit 0

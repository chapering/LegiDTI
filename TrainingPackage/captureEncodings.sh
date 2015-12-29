#!/bin/bash

function part1()
{
	for ((i=1;i<=8;++i))
	do
		/home/chap/dtt-legibility/encodingStudy_training/bin/task1/LegiDTI -f \
		/home/chap/dtt-legibility/encodingStudy_training/data/normal/region_for_task1/s3/cst/region_s3.data -s \
		/home/chap/dtt-legibility/encodingStudy_training/bin/task1/valueSettings${i}.xml -r 0.2 -i 1
	done
}

function part2()
{
	for ((i=1;i<=3;++i))
	do
		/home/chap/dtt-legibility/encodingStudy_training/bin/task2/LegiDTI -f \
		/home/chap/dtt-legibility/encodingStudy_training/data/normal/region_for_task1/s3/cst/region_s3.data -s \
		/home/chap/dtt-legibility/encodingStudy_training/bin/task2/valueSettings${i}.xml -r 0.2 -i 1
	done
}

part1
part2

echo "all finished."

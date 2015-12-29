#!/bin/bash
: '
this script is to set up by building the seeding method study; 
'

BASEDIR=`pwd`
SRCDIR=${BASEDIR}/src
BINDIR=${BASEDIR}/bin
DATADIR=${BASEDIR}/data
EXECUTABLES="LegiDTI"
#TASKDIRS=${1:-"pretask task1 task2 task3 task4 task5 task6"}
TASKDIRS=${1:-"pretask task1 task2 task5"}

if [ ! -d ${SRCDIR} ];then
	echo "FATAL: directory ${SRCDIR} NOT found."
	exit -1
fi

cd ${SRCDIR}
if [ ! -d CMakeFiles ];then
	ccmake .
	cmake .
fi
cd -

: '
build executables for each of the tasks and set them up in task-wise
sub-directories under the $BIN directory
'
mkdir -p ${BINDIR} ${DATADIR}
for ts in ${TASKDIRS}
do
	echo -e "building for ${ts}..."
	mkdir -p ${BINDIR}/${ts}

	if [ ! -d ${SRCDIR}/${ts} ];then
		echo "FATAL: sub-directory ${SRC}/${ts} NOT found."
		rm -rf ${BINDIR}/*
		exit -1
	fi

	cp ${SRCDIR}/${ts}/*.{h,cpp} ${SRCDIR}
	make -C ${SRCDIR}

	for bin in ${EXECUTABLES}
	do
		if [ ! -s ${SRCDIR}/${bin} ];then
			echo "ERROR: executable ${SRCDIR}/${bin} for ${ts} failed to be built."
			rm -rf ${BINDIR}/*
			exit -1
		fi
		mv -f ${SRCDIR}/${bin} ${BINDIR}/${ts}
	done
	make -C ${SRCDIR} -w clean

	cp ${SRCDIR}/${ts}/tasktext ${BINDIR}/${ts}
	cp ${SRCDIR}/${ts}/*.xml ${BINDIR}/${ts}

	echo -e "\t\t------ finished."
done

echo -e "\n\t---- setup completed ----\t\n"

exit 0


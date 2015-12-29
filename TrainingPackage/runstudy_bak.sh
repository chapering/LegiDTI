BASEDIR=`pwd`

BINDIR=${BASEDIR}/bin
#TASKDIRS="pretask task1 task2 task3 task4 task5 task6"
TASKDIRS="pretask task1 task2 task5"
EXECUTABLES="LegiDTI"

DATADIR=${BASEDIR}/data
#ALLNORMALCASEDATADIRS="${DATADIR}/normal_whole ${DATADIR}/normal_allfb"
ALLNORMALCASEDATADIRS=${DATADIR}/normal
ALLFBDATADIR=${DATADIR}/allfbs/
#y=`expr $RANDOM % 100`
#if [ $y -ge 50 ];then
#	ALLNORMALCASEDATADIRS="${DATADIR}/normal_allfb ${DATADIR}/normal_whole"
#fi

BUNDLES="cst cc ifo ilf"
FIBERBUNDLES=(cst cc ifo ilf)

LOG=${BASEDIR}/studylog

ORDERARRAY=()
NUMROW=0 #number of row in the order matrix
NUMCOL=0 #number of column in the order matrix
#FBARRARY=(cst cc ifo ilf)
CURORDER=""
CURFB=""

TASKPROGIDFILE=${BASEDIR}/taskprogidfile
TASKNO=0

TUBERADIUS="0.2"

encodingorder0=(1 2 3 4 5 6 7 8)
encodingorder1=(2 3 4 5 6 7 8 1)
encodingorder2=(3 4 5 6 7 8 1 2)
encodingorder3=(4 5 6 7 8 1 2 3)
encodingorder4=(5 6 7 8 1 2 3 4)
encodingorder5=(6 7 8 1 2 3 4 5)
encodingorder6=(7 8 1 2 3 4 5 6)
encodingorder7=(8 1 2 3 4 5 6 7)

encodingorderm0=(1 2 3 4 5 6)
encodingorderm1=(2 3 4 5 6 1)
encodingorderm2=(3 4 5 6 1 2)
encodingorderm3=(4 5 6 1 2 3)
encodingorderm4=(5 6 1 2 3 4)
encodingorderm5=(6 1 2 3 4 5)
encodingorderm6=(1 2 3 4 5 6)
encodingorderm7=(2 3 4 5 6 1)

function checkbins()
{
	if [ ! -d ${BINDIR} ];then
		echo "FATAL: directory ${BINDIR} NOT found, please set up firstly."
		return 1
	fi

	for ts in ${TASKDIRS}
	do
		for bin in ${EXECUTABLES}
		do
			if [ ! -s ${BINDIR}/${ts}/${bin} ];then
				echo "ERROR: executable ${bin} for ${ts} NOT found,"
			    echo "please set up firstly."
				return 1
			fi
		done
	done
	return 0
}

function checkdata()
{
	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		for bdir in ${NORMALCASEDATADIR}
		#${ABNORMALCASEDATADIR}
		do
			if [ ! -d ${bdir} ];then
				echo "ERROR: data directory ${bdir} NOT found."
				return 1
			fi

			# check task-specific data
			for ts in ${TASKDIRS}
			do
				if [ $ts = "pretask" ];then
					continue
				fi

				taskdatadir="region_for_${ts}"
				for ((n=${fixres};n<=${fixres};n++));do
					for fb in ${BUNDLES}
					do
						# cc and ifo not used in task6
						if [ $ts = "task6" ];then
							if [ $fb = "cc" -o $fb = "ifo" ];then
								continue
							fi
						fi

						if [ `ls -R ${bdir}/${taskdatadir}/s${n}/${fb}/ | grep data | wc -l` -lt 2 ];then
							echo "ERROR: data missed for ${ts}/s${n}/${fb}."
							return 1
						fi
					done
				done
			done
		done
	done

	return 0
}

function readorder()
{
	if [ $# -lt 1 -o ! -s $1 ];then
		echo "In readorder: too few arguments or file does not exist."
		return 1
	fi

	# take the number of fields in the first line as the number of column  of the matrix to read into
	local fldnum=0
	local currow=0
	#cat $1 | \
	while read curline
	do
		curfldnum=`echo $curline | awk '{print NF}'`
		if [ $fldnum -eq 0 ];then
			fldnum=$curfldnum
		elif [ $curfldnum -ne $fldnum ];then
			echo "inconsistent row - different number of columns."
			return 1
		fi

		local currol=0
		for num in $curline
		do
			let "index = $currow * $fldnum + $currol"
			ORDERARRAY[$index]=$num
			let "currol += 1"
		done

		let "currow += 1"

	done < $1

	let NUMROW=$currow
	let NUMCOL=$fldnum
	#echo "NUMCOL=$NUMCOL"
	#echo "NUMROW=$NUMROW"

	return 0
}

function print_order()
{
	echo ${ORDERARRAY[*]} | xargs -n $NUMCOL
}

function taskflag()
{
	echo -e "\n##############################################################" >> $LOG
	echo "                           TASK $1                            " >> $LOG
	echo -e "##############################################################\n" >> $LOG
}

function updatetaskprog()
{
	let "TASKNO += 1"
	echo "$TASKNO out of 8" > ${TASKPROGIDFILE}
}

#-----------------------------------------#
#
# pretask
#
#-----------------------------------------#
function pretask()
{
	${BINDIR}/singleitr \
	-u \
	${ALLFBDATADIR}/allfbs_colored.data \
	-f \
	${ALLFBDATADIR}/normal_white.data \
	-s \
	${ALLFBDATADIR}/skeleton_allfb.data \
	-j \
	${ALLFBDATADIR}/tumorbox_cst.data \
	-j \
	${ALLFBDATADIR}/tumorbox_cg.data \
	-j \
	${ALLFBDATADIR}/tumorbox_cc.data \
	-j \
	${ALLFBDATADIR}/tumorbox_ifo.data \
	-j \
	${ALLFBDATADIR}/tumorbox_ilf.data 

	# this is a trivial, actually virtual, task. It just show the introductory message of the tasks to follow
	${BINDIR}/pretask/LegiDTI \
		-t ${BINDIR}/pretask/tasktext \
		-V 1>> $LOG 2>&1
}

#-----------------------------------------#
#
# task 1
#
#-----------------------------------------#
function task1()
{
	taskflag 1

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0

		echo "##### data category ${NORMALCASEDATADIR} #####" >> $LOG
		settingIdx=0
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$((n-1))]}
			CURFB=${FBARRARY[$di]}
			#CURFB="cst"
			let "di += 1"
			flip=0
			if [ "$CURFB" == "cst" ];then
				flip=1
			fi

			settingID=$(eval echo \${encodingorder${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			${BINDIR}/task1/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/region_s${fixres}.data \
			-s ${BINDIR}/task1/valueSettings${settingID}.xml \
			-j \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/tumorbox_0_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/tumorbox_1_region_s${fixres}.data \
			-t ${BINDIR}/task1/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-i ${flip} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi

			let "settingIdx += 1"
			if [ $settingIdx -gt 2 ];then
				break
			fi
		done

		if [ $settingIdx -gt 2 ];then
			continue	
		fi

		# the same participant does the second block of task data
		FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$n]}
			CURFB=${FBARRARY[$di]}
			let "di += 1"
			flip=0
			if [ "$CURFB" == "cst" ];then
				flip=1
			fi

			settingID=$(eval echo \${encodingorder${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			${BINDIR}/task1/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-s ${BINDIR}/task1/valueSettings${settingID}.xml \
			-j \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/pos1/tumorbox_0_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task1/s${fixres}/${CURFB}/pos1/tumorbox_1_region_s${fixres}.data \
			-t ${BINDIR}/task1/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-i ${flip} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
			let "settingIdx += 1"
			if [ $settingIdx -gt 7 ];then
				break
			fi
		done
	done
	return 0
}

#-----------------------------------------#
#
# task 2 
#
#-----------------------------------------#
function task2()
{
	taskflag 2

	local keys1=(2 2 3 3 2)
	local keys2=(3 2 1 3 1)
	local keys3=(3 2 3 2 3)
	local keys4=(2 2 3 2 3)

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		sfb=(cst cc ifo ilf)
		dices=( $(echo "0 1 2 3" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		FBARRARY=( ${sfb[${dices[0]}]} ${sfb[${dices[1]}]} ${sfb[${dices[2]}]} ${sfb[${dices[3]}]} )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0

		echo "##### data category ${NORMALCASEDATADIR} #####" >> $LOG
		settingIdx=0
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURKEY=$(eval echo \${keys$((n))[$((fixres-1))]})
			#CURFB=${FIBERBUNDLES[$n]}

			CURKEY=$(eval echo \${keys$((${dices[$di]}+1))[$((fixres-1))]})
			CURFB=${FBARRARY[$di]}
			#CURFB="cc"
			let "di += 1"

			settingID=$(eval echo \${encodingorderm${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			${BINDIR}/task2/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/region_s${fixres}.data \
			-s ${BINDIR}/task2/valueSettings${settingID}.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/fiberidx_*_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/tumorbox_0_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/tumorbox_1_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/tumorbox_2_region_s${fixres}.data \
			-t ${BINDIR}/task2/tasktext \
			-k ${CURKEY} \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
			let "settingIdx += 1"
			if [ $settingIdx -gt 1 ];then
				break
			fi
		done

		if [ $settingIdx -gt 1 ];then
			continue	
		fi

		# the same participant does the second block of task data
		sfb=(cst cc ifo ilf)
		dices=( $(echo "0 1 2 3" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		FBARRARY=( ${sfb[${dices[0]}]} ${sfb[${dices[1]}]} ${sfb[${dices[2]}]} ${sfb[${dices[3]}]} )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0

		for n in ${CURORDER}
		do
			updatetaskprog
			#CURKEY=$(eval echo \${keys$((n))[$((fixres-1))]})
			#CURFB=${FIBERBUNDLES[$n]}

			CURKEY=$(eval echo \${keys$((${dices[$di]}+1))[$((fixres-1))]})
			CURFB=${FBARRARY[$di]}
			let "di += 1"

			settingID=$(eval echo \${encodingorderm${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			${BINDIR}/task2/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-s ${BINDIR}/task2/valueSettings${settingID}.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/fiberidx_*_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/tumorbox_0_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/tumorbox_1_region_s${fixres}.data \
			-j \
			${NORMALCASEDATADIR}/region_for_task2/s${fixres}/${CURFB}/pos1/tumorbox_2_region_s${fixres}.data \
			-t ${BINDIR}/task2/tasktext \
			-k ${CURKEY} \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
			let "settingIdx += 1"
			if [ $settingIdx -gt 5 ];then
				break
			fi
		done
	done
	return 0
}

#-----------------------------------------#
#
# task 3 
#
#-----------------------------------------#
function task3()
{
	taskflag 3

	sfb=(cst cc ifo ilf)
	dices=( $(echo "0 1 2 3" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
	FBARRARY=( ${sfb[${dices[0]}]} ${sfb[${dices[1]}]} ${sfb[${dices[2]}]} ${sfb[${dices[3]}]} )
	echo "focus order:" ${FBARRARY[*]} >> $LOG
	local di=0

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$n]}
			CURFB=${FBARRARY[$di]}

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			${BINDIR}/task3/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/region_s${fixres}.data \
			-s ${BINDIR}/task3/valueSettings.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/fiberidx_*_region_s${fixres}.data \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-k $((${dices[$di]}+1)) \
			-t ${BINDIR}/task3/tasktext \
			-V 1>> $LOG 2>&1

			let "di += 1"
			if [ $? -ne 0 ];then
				return 3
			fi
		done

		# the same participant does the second block of task data
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$n]}
			CURFB=${FBARRARY[$di]}

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			${BINDIR}/task3/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-s ${BINDIR}/task3/valueSettings.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task3/s${fixres}/${CURFB}/pos1/fiberidx_*_region_s${fixres}.data \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-k $((${dices[$di]}+1)) \
			-t ${BINDIR}/task3/tasktext \
			-V 1>> $LOG 2>&1

			let "di += 1"

			if [ $? -ne 0 ];then
				return 3
			fi

		done
	done
	return 0
}

#-----------------------------------------#
#
# task 4 
#
#-----------------------------------------#
function task4()
{
	taskflag 4

	FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
	echo "focus order:" ${FBARRARY[*]} >> $LOG
	local di=0

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$n]}
			CURFB=${FBARRARY[$di]}
			let "di += 1"

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			${BINDIR}/task4/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/region_s${fixres}.data \
			-s ${BINDIR}/task4/valueSettings.xml \
			-j \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/tumorbox_0_region_s${fixres}.data \
			-t ${BINDIR}/task4/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
		done

		# the same participant does the second block of task data
		for n in ${CURORDER}
		do
			updatetaskprog
			#CURFB=${FIBERBUNDLES[$n]}
			CURFB=${FBARRARY[$di]}
			let "di += 1"

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			${BINDIR}/task4/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-s ${BINDIR}/task4/valueSettings.xml \
			-j \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/pos1/tumorbox_0_region_s${fixres}.data \
			-t ${BINDIR}/task4/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
		done
	done
	return 0
}

#-----------------------------------------#
#
# task 5 
#
#-----------------------------------------#
function task5()
{
	taskflag 5

	#local binchoices=(no yes yes no yes)
	local binchoices1=(yes no no no yes)
	local binchoices2=(yes yes no yes yes)
	local binchoices3=(no no no no yes)
	local binchoices4=(yes yes yes yes no)
	local binchoices5=(no no no no yes)
	local binchoices6=(yes yes no yes no)
	local binchoices7=(no no no yes yes)
	local binchoices8=(yes yes no no yes)
	local binchoices9=(yes no yes no yes)
	local binchoices10=(yes yes no yes yes)

	FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
	echo "focus order:" ${FBARRARY[*]} >> $LOG
	local di=0

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0

		echo "##### data category ${NORMALCASEDATADIR} #####" >> $LOG
		settingIdx=5

		let "pidx = pi + 1"
		let "pidx2 = pidx + $NUMCOL"

		for ((i=0;i<$NUMCOL;i++))
		do
			updatetaskprog

			#let "index = $pi * $NUMCOL + $i"
			#n=${ORDERARRAY[index]}
			#CURFB=${FIBERBUNDLES[$n]}

			CURFB=${FBARRARY[$di]}
			#CURFB="ilf"
			CURCHOICE=$(eval echo \${binchoices${pidx}[$di]})

			let "di += 1"

			settingID=$(eval echo \${encodingorder${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			echo "#####    when the answer is $CURCHOICE #### ">> $LOG
			FIBERIDXFILE="fiberidx_*_region_s${fixres}.data"
			#if [ "$CURCHOICE" == "no" ];then
				#FIBERIDXFILE="fiberidx_compound_region_s${fixres}.data"
			#fi

			#-s ${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			${BINDIR}/task5/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/region_s${fixres}.data \
			-s ${BINDIR}/task5/valueSettings${settingID}.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/${CURCHOICE}/${FIBERIDXFILE} \
			-t ${BINDIR}/task5/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
			let "settingIdx += 1"
			if [ $settingIdx -gt 7 ];then
				break
			fi
		done

		if [ $settingIdx -gt 7 ];then
			continue	
		fi

		# the same participant does the second block of task data
		FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
		echo "focus order:" ${FBARRARY[*]} >> $LOG
		local di=0

		for ((i=0;i<$NUMCOL;i++))
		do
			updatetaskprog

			#let "index = $pi * $NUMCOL + $i"
			#n=${ORDERARRAY[index]}
			#CURFB=${FIBERIDXFILE[$n]}
			#CURCHOICE=$(eval echo \${binchoices${pidx2}[$i]})

			CURFB=${FBARRARY[$di]}
			CURCHOICE=$(eval echo \${binchoices${pidx}[$di]})

			let "di += 1"

			settingID=$(eval echo \${encodingorder${pi}[$settingIdx]})

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			echo "##### value setting $settingID #####" >> $LOG
			echo "#####    when the answer is $CURCHOICE #### ">> $LOG
			FIBERIDXFILE="fiberidx_*_region_s${fixres}.data"
			#if [ "$CURCHOICE" == "no" ];then
			#	FIBERIDXFILE="fiberidx_compound_region_s${fixres}.data"
			#fi

			#-s ${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			${BINDIR}/task5/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-s ${BINDIR}/task5/valueSettings${settingID}.xml \
			-i \
			${NORMALCASEDATADIR}/region_for_task5/s${fixres}/${CURFB}/pos1/${CURCHOICE}/${FIBERIDXFILE} \
			-t ${BINDIR}/task5/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
			let "settingIdx += 1"
			if [ $settingIdx -gt 7 ];then
				break
			fi
		done
	done
	return 0
}

#-----------------------------------------#
#
# task 6
#
#-----------------------------------------#
function task6()
{
	taskflag 6

	FBARRARY=( $(echo "cst cc ifo ilf" | sed -r 's/(.[^;]*;)/ \1 /g' | tr " " "\n" | shuf | tr -d " " ) )
	echo "focus order:" ${FBARRARY[*]} >> $LOG
	local di=0

	for NORMALCASEDATADIR in  ${ALLNORMALCASEDATADIRS}
	do
		for n in ${CURORDER}
		do
			updatetaskprog
			CURFB=${FBARRARY[$di]}
			let "di += 1"

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			m=`expr $RANDOM % 5`
			let "m++"
			echo "compared with seeding resolution of ${m}x${m}x${m}, bundle of $CURFB" >> $LOG
			key=1
			if [ $m -ne ${fixres} ];then
				key=2
			fi
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/skeleton_region_s${fixres}.data \
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${m}/${CURFB}/skeleton_region_s${m}.data \
			${BINDIR}/task6/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/region_s${fixres}.data \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${m}/${CURFB}/region_s${m}.data \
			-s ${BINDIR}/task6/valueSettings.xml \
			-t ${BINDIR}/task6/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-k ${key} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
		done

		# the same participant does the second block of task data
		for n in ${CURORDER}
		do
			updatetaskprog
			CURFB=${FBARRARY[$di]}
			let "di += 1"

			echo -e "\n##### with Normal case #####" >> $LOG
			echo "##### under seeding resolution of ${fixres}x${fixres}x${fixres} #####" >> $LOG
			m=`expr $RANDOM % 5`
			let "m++"
			echo "compared with seeding resolution of ${m}x${m}x${m}, bundle of $CURFB" >> $LOG
			key=1
			if [ $m -ne ${fixres} ];then
				key=2
			fi
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/pos1/skeleton_region_s${fixres}.data \
			#-s ${NORMALCASEDATADIR}/region_for_task4/s${m}/${CURFB}/pos1/skeleton_region_s${m}.data \
			${BINDIR}/task6/LegiDTI \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${fixres}/${CURFB}/pos1/region_s${fixres}.data \
			-f \
			${NORMALCASEDATADIR}/region_for_task4/s${m}/${CURFB}/pos1/region_s${m}.data \
			-s ${BINDIR}/task6/valueSettings.xml \
			-t ${BINDIR}/task6/tasktext \
			-p ${TASKPROGIDFILE} \
			-r ${TUBERADIUS} \
			-k ${key} \
			-V 1>> $LOG 2>&1

			if [ $? -ne 0 ];then
				return 3
			fi
		done
	done
	return 0
}

function costOfThisTask()
{
	s=$1
	e=$2
	((d=e-s))
	((h=d/3600))
	((m=d%3600/60))
	((s=d%3600%60))
	echo " Time cost of this task: $h hours $m minutes $s seconds." >> $LOG
}

function execTasks()
{
	> $LOG

	bstart=`date +%s`

	for curtask in ${TASKDIRS}
	do 
		curstart=`date +%s`
		$curtask	
		ret=$?
		curend=`date +%s`
		costOfThisTask ${curstart} ${curend}

		if [ $ret -ne 0 ];then
			return 3
		fi
	done

	end=`date +%s`
	echo -e "\n################## ALL FINISHED #######################" >> $LOG
	((d=end-bstart))
	((h=d/3600))
	((m=d%3600/60))
	((s=d%3600%60))
	echo " Time cost: $h hours $m minutes $s seconds." >> $LOG
	return 0
}

#####################################################################################
###    Task ordering and main flow control
###
#####################################################################################

checkbins
if [ $? -ne 0 ];then
	exit 1
fi

checkdata
if [ $? -ne 0 ];then
	exit 1
fi

if [ $# -lt 1 ];then
	echo "Usage: $0 <participant index> [resolution]"
	exit 1
fi

pi=$1

fixres=3
if [ $# -ge 2 ];then
	fixres=$2
fi

LOG="${LOG}_p${pi}_legi"
let "pi -= 1"

NUMCOL=4
CURORDER="1 2 3 4"

echo ${ALLNORMALCASEDATADIRS[*]} >> $LOG
execTasks
ret=$?

rm -rf ${TASKPROGIDFILE}

if [ $ret -eq 0 ];then
	echo "All finished, thank you!"
else
	echo "Study terminated in advance, thank you all the same."
fi
echo

exit 0


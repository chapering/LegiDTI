#!/usr/bin/env python

import os
import sys
import string
import re
from odict import *
from math import fabs
from os import listdir
from os.path import isfile, join

# name of file to parse
g_fns=[]

# hole all streamlines
g_allimfiles=[]
g_bKeepImfile=False

'''data structure:
	[ {taskid:{trialid:[(dcls, encoding, singletasktime, hesitime, did, correctness, task answer),...], ...}, ...}, ... ]
'''
g_allparticipantinfo=[]

encoding_schemes13={1:7, 2:1, 3:27, 4:2, 5:4, 6:5, 7:6, 8:17}
encoding_schemes2={1:8, 2:3, 3:2,  4:6, 5:4, 6:5}

split=string.split

PREPROCESS_FILTER="TASK|Task completed|Loading finished|hightlighted|focus order|data category|value setting"
# keywords for retrieving feature lines
FORTASK="TASK"
FORANSWER="Task completed with Answer"
FORSTART="Loading finished"
FORHLOPT="hightlighted"
FORFBFOCUS="focus order:"
FORDCLASS="data category"
FORENCODING="value setting"

TASK2KEYS=(
	(2, 2, 3, 3, 2),
	(1, 2, 2, 1, 2),
	(3, 2, 1, 3, 1),
	(3, 2, 3, 2, 3),
	(2, 2, 3, 2, 3))

TASK5KEYS=(
	((1, 2, 2, 2, 1),
	(1, 1, 2, 1, 1),
	(2, 2, 2, 2, 1),
	(1, 1, 1, 1, 2),
	(2, 2, 2, 2, 1)),

	((1, 1, 2, 1, 2),
	(2, 2, 2, 1, 1),
	(1, 1, 2, 2, 1),
	(1, 2, 1, 2, 1),
	(1, 1, 2, 1, 1)))

#FBNAMES={"cst":1, "cg":2, "cc":3, "ifo":4, "ilf":5}
FBNAMES={"cst":1, "cc":2, "ifo":3, "ilf":4}

# will load these two external reference if source file can be found in the
# current directory
g_allFADiffs=[]
g_allBundlesizeDiffs=[]
g_allmfiber_t2=[]
g_allmfiber_t3=[]
g_alllesioninfo=[]
g_allSNinfo=[]

g_columndatadir="./columnData/"

## @brief print usage
## @param none
## @retrun none
def Usage():
	print >> sys.stdout, "%s <run log> \n" % \
			sys.argv[0]
	return

## @brief parse command line to get user's input for source file name and target
## @param none
## @retrun none
def ParseCommandLine():
	global g_fns
	global g_allparticipantinfo
	argc = len(sys.argv)
	if argc >= 2:
		for i in range(1, argc):
			g_fns.append( os.path.abspath(sys.argv[i]) )
			if not os.path.exists( g_fns[i-1] ):
				raise IOError, "source file given [%s] does not exist, bailed out now." \
					% g_fns[i-1]
			g_allparticipantinfo.append( dict() )
	else:
		Usage()
		raise Exception, "too few arguments, aborted."

	'''
	for i in range(1,10):
		if os.path.isfile("%s/faDiffs_block%d" % (g_columndatadir,i)):
			hfFA=file("%s/faDiffs_block%d" % (g_columndatadir,i),'r')
			g_allFADiffs.append(hfFA.readlines())
			hfFA.close()
		else:
			break

	for i in range(1,10):
		if os.path.isfile("%s/bundleDiffs_block%d" % (g_columndatadir,i)):
			hfbd=file("%s/bundleDiffs_block%d" % (g_columndatadir,i))
			g_allBundlesizeDiffs.append(hfbd.readlines())
			hfbd.close()
		else:
			break

	for i in range(1,10):
		if os.path.isfile("%s/task2_nmfb_block%d" % (g_columndatadir,i)):
			hfmfb=file("%s/task2_nmfb_block%d" % (g_columndatadir,i),'r')
			g_allmfiber_t2.append(hfmfb.readlines())
			hfmfb.close()
		else:
			break

	for i in range(1,10):
		if os.path.isfile("%s/task5_SN_block%d" % (g_columndatadir,i)):
			hfsn=file("%s/task5_SN_block%d" % (g_columndatadir,i))
			g_allSNinfo.append(hfsn.readlines())
			hfsn.close()
		else:
			break
	'''

## @brief preprocess the raw log and filter out only relevant information 
##	for the statistics
## @param srcfns a list of raw log files
## @return none
def preprocess(srcfns):
	global g_allimfiles
	for rfn in srcfns:
		imfn = "%s_stat" % os.path.abspath(rfn)
		os.system("cat %s | grep -E \"%s\" | grep -v \"elements\" > %s" % (os.path.abspath(rfn), PREPROCESS_FILTER, imfn) )
		g_allimfiles.append( imfn )
		print >> sys.stderr, "%s processed and procuded %s" % (rfn, imfn)

## @brief load the run log for a single participant
## @param srcfn the run log file of a single participant
## @return none
def loadsingle(srcfn, pidx, trialid):
	def pickTime( curline ) :
		lasttime = -1
		curline = curline.lstrip().rstrip(' \r\n')
		if len(curline) > 10 and curline[0] == '[':
			timeinfo = split( curline[1:] )
			mssec = int( timeinfo[1] )
			timeinfo = split( timeinfo[0], ':', 2 )
			lasttime = int( timeinfo[0] ) * 3600 + int( timeinfo[1] ) * 60 + int (timeinfo[2]) + mssec*1.0/1000
		return lasttime

	def pickAnswer( curline ) :
		answer = -1
		tanswer = " "
		curline = curline.lstrip().rstrip(' \r\n')
		if len (curline) > 10 and string.find( curline, FORANSWER) != -1:
			ansinfo = split ( curline )
			if len(ansinfo) == 8 :
				# answer not available
				return -2
			tanswer = ansinfo[-2]
			answer = ansinfo[-1]
			if answer == "(correct).":
				answer = 1
			elif answer == "(wrong).":
				answer = 0
		return (answer, tanswer)

	def pickTaskId( curline ):
		taskId = -1
		curline = curline.lstrip().rstrip().rstrip('\r\n')
		taskinfo = split( curline )
		if len(taskinfo) == 2 and taskinfo[0] == FORTASK:
			taskId = int( taskinfo[1] )
			if 5 == taskId:
				taskId = 3
		return taskId

	def pickFocusOrder ( curline ):
		curorder = []
		curline = curline.lstrip().rstrip().rstrip('\r\n')
		orderinfo = split ( curline[ curline.find(FORFBFOCUS) + len(FORFBFOCUS) + 1:] )
		if len(orderinfo) == len(FBNAMES):
			curorder = orderinfo
		return curorder

	def pickDataCls( curline ):
		dcls = -1
		curline = curline.lstrip().rstrip().rstrip('\r\n')
		taskinfo = split( curline[ curline.find(FORDCLASS) + len(FORDCLASS) + 1:] )
		if len(taskinfo) == 2:
			dirnodes = split( taskinfo[0], "/" )
			if dirnodes[ -1 ] == "normal_allfb":
				dcls = 0
			elif dirnodes[ -1 ] == "normal_whole":
				dcls = 1
		return dcls 

	def pickEncoding( curline ):
		encoding = -1
		curline = curline.lstrip().rstrip().rstrip('\r\n')
		taskinfo = split( curline[ curline.find(FORENCODING) + len(FORENCODING) + 1:] )
		if len(taskinfo) == 2:
			encoding = int ( taskinfo[0] )
		return encoding 

	sfh = file(srcfn,"r")
	if None == sfh:
		raise Exception, "Failed to open file - %s." % (srcfn)

	lnCnt = 0
	lasttime = 0
			
	''' read one task by another '''
	taskid=None
	optionlogs=[]
	curfbindices=[]
	fbidx=0
	dcls=None
	encoding=None

	#participantInfo = dict()
	global g_allparticipantinfo
	participantInfo = g_allparticipantinfo[pidx]

	curline = sfh.readline()
	# overlook lines before the first TASK tag
	while string.find( curline, FORTASK ) == -1:
		lnCnt += 1
		curline = sfh.readline()

	while curline:
		lnCnt += 1
		if string.find( curline, FORTASK ) != -1:
			taskid = pickTaskId( curline )
			if taskid == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)
			curline = sfh.readline()
			continue

		if string.find( curline, FORFBFOCUS ) != -1:
			curfbseq = pickFocusOrder( curline )
			#print "curfbseq: %s" % curfbseq
			curfbindices=[]
			for fb in curfbseq:
				curfbindices.append( FBNAMES[fb] )
			fbidx=0
			curline = sfh.readline()
			continue

		if string.find( curline, FORDCLASS ) != -1:
			dcls = pickDataCls( curline )
			if dcls == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)
			curline = sfh.readline()
			continue

		if string.find( curline, FORENCODING ) != -1:
			encoding = pickEncoding( curline )
			if encoding == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)
			curline = sfh.readline()
			continue

		if string.find( curline, FORSTART ) != -1:
			lasttime = pickTime( curline )
			if lasttime == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)
			curline = sfh.readline()
			continue

		if string.find( curline, FORHLOPT ) != -1:
			optionlogs.append(curline)
			curline = sfh.readline()
			continue

		if string.find( curline, FORANSWER ) != -1:
			finishtime = pickTime( curline )
			if finishtime == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)
			singletasktime = finishtime - lasttime
			#lasttime = finishtime

			(answer, tanswer) = pickAnswer( curline )
			if answer == -1:
				raise ValueError, "parsing %s failed at line No. %d - %s." % (srcfn,lnCnt,curline)

			ttans=tanswer
			if tanswer=="hit" or tanswer=="away":
				ttans="1"
				
			FOROPTTIME="option %s hightlighted" % (int(ttans)-1)
			optionlogs.reverse()
			toptidx = -1
			for optlidx in range(0, len(optionlogs)):
				if -1 != optionlogs[optlidx].rfind( FOROPTTIME ):
					toptidx = optlidx
					break

			if toptidx == -1:
				raise ValueError, "parsing %s failed at line No. %d - matching the last option hightlighted lines." % (srcfn,lnCnt)
			optiontime = pickTime( optionlogs[toptidx] )
			if optiontime == -1:
				raise ValueError, "parsing %s failed at line No. %d - extracting last option hightlighted time." % (srcfn,lnCnt)

			optionlogs = []
			hesitime = finishtime - optiontime

			# now information for a single task collected, store it
			if taskid not in participantInfo.keys():
				participantInfo[ taskid ] = odict()
			if trialid not in participantInfo[ taskid ].keys():
				participantInfo[ taskid ][ trialid ] = list()

			participantInfo[ taskid ][ trialid ].append( (dcls, encoding, singletasktime, hesitime, curfbindices[fbidx], answer, tanswer) )

			fbidx += 1
			if fbidx > len(FBNAMES):
				fbidx = 0
			curline = sfh.readline()
			continue

		print >> sys.stderr, "line skipped - %s" % curline
		curline = sfh.readline()
			
	sfh.close()
	print >> sys.stderr, "%d lines loaded in file %s" % (lnCnt, srcfn)
	#global g_allparticipantinfo
	#g_allparticipantinfo.append ( participantInfo )
	#g_allparticipantinfo[pidx] = (pidx, participantInfo)
	#g_allparticipantinfo.insert(pidx, participantInfo)

def dirfiles(mypath):
	onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]
	ret=[]
	for fn in onlyfiles:
		ret.append( os.path.abspath(mypath) + "/" + fn )
	return ret 

## @brief read the run logs for all participants
## @param imfns the list of immediate files
def loadall(srcfns):
	pidx = 0
	for dname in srcfns:
		dfiles = dirfiles( os.path.abspath(dname) )
		preprocess(dfiles)

		#try:
		trid=1
		for srcfn in g_allimfiles:
			loadsingle( srcfn, pidx, trid )
			trid += 1
		#except Exception, e:
			#print >> sys.stderr, "Failed to parse source files : %s" % (e)

		pidx += 1
		print >> sys.stderr, "%d participants have been analyzed." % (pidx)
		if not g_bKeepImfile:
			removeImfiles()

## @brief serialize pariticipant info to the stdout file
def dump():
	global g_allparticipantinfo
	global g_allFADiffs
	global g_allBundlesizeDiffs

	''' this is title line '''
	DataCategories=['CST','CC','IFO','ILF']
	#print >> sys.stdout, "Pid Trialid Tid Dcls Did Bname Encoding Time htime Correct Ans Key ec1 ec2 ec3 ec4 ec5"
	#print >> sys.stdout, "Pid Trialid Tid Dcls Did Bname Encoding Time Correct"
	print >> sys.stdout, "Pid Order Trialid Tid Dcls Encoding Time Correct"
	for pidx in range( 0, len( g_allparticipantinfo ) ):
		tnum=1 # task execution order
		for tid in g_allparticipantinfo[pidx].keys():
			for trialid in g_allparticipantinfo[pidx][tid].keys():
				for ta in g_allparticipantinfo[pidx][tid][trialid]:
					'''
					bfid=int(ta[2])
					did=bfid
					dprop=""
					key=0
					if tid == 1 and len(g_allFADiffs)>blockId: # FA difference
						dprop= g_allFADiffs[blockId][ (did-1)*5+int(res[0])-1 ] 
						dprop_words=split(dprop)
						assert ( len(dprop_words) >= 5 )
						fa1 = float(dprop_words[1])
						fa2 = float(dprop_words[3])
						if fabs( fa1 - fa2 ) < 1e-2:
							key=1
						elif fa1 > fa2:
							key=2
						else:
							key=3

					elif tid == 2 and len(g_allmfiber_t2)>blockId: # number of marked fibers
						dprop="%s 0 0 0 0"%( g_allmfiber_t2[blockId][ (did-1)*5+int(res[0])-1 ].rstrip("\r\n") )
						key=TASK2KEYS[did-1][int(res[0])-1]

					elif tid == 3:
						dprop="%s 0 0 0" % ( g_allSNinfo[blockId][(did-1)*5+int(res[0])-1].rstrip("\r\n") )
						#key=TASK5KEYS[blockId%2][did-1][int(res[0])-1]
						if ta[3] == 1:
							key = ta[4]
						else:
							key = 3 - int(ta[4])
					'''
					encm=int(ta[1])
					if tid==1 or tid==3:
						encm=encoding_schemes13[encm]
					elif tid==2:
						encm=encoding_schemes2[encm]

					print >> sys.stdout, "%d %d %d %d %d %d %d %d" % \
						(pidx+1, tnum, trialid, tid, ta[0], encm, ta[2], ta[5])

					tnum+=1

## @brief remove all immediate files
def removeImfiles():
	global g_allimfiles
	for imfn in g_allimfiles:
		os.system(" rm -f %s " % (imfn) )
	g_allimfiles = []

######################################
# the boost
if __name__ == "__main__":
	try:
		ParseCommandLine()
	except Exception,e:
		print >> sys.stderr, e
		sys.exit(1)

	loadall( g_fns )
	dump()
	sys.exit(1)
		
# set ts=4 tw=100 sts=4 sw=4
 

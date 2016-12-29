#!/usr/bin/python

import os
import sys

def rec_test (str, cmd, check, data):
	print "\n* Recursive test of " + str
	test(str, cmd, check)
	for i in list_cmd.items():
		test(i[0], i[1][0], i[1][1])

def test (str, cmd, check):
	print "Test:", str,

	i=0
	while i < repeat:
		ret=os.system(somaclient + ssl + " -S " + server + 
		" -p" + password + " " + cmd + " 2>&1 >/dev/null")
		if check == 0:
			if ret != 0:
				print " Failed!"
				break
		print ".",
		i+=1
	else:
		print "Ok!"

def usage():
	print "Usage:\n\t", sys.argv[0], \
		"<server> <password> <item> <repeat> <somaclient> <ssl: 0 or 1>"

password=""
server=""
somaclient=""
item=""
repeat=0
ssl=""

if len(sys.argv) == 1:
	usage()
else:
	for i in range(len(sys.argv)):
		if i == 1: server=sys.argv[i]
		if i == 2: password=sys.argv[i]
		if i == 3: item=sys.argv[i]
		if i == 4: 
			try:
				repeat=int(sys.argv[i])
			except ValueError:
				repeat=1
		if i == 5: somaclient=sys.argv[i]
		if i == 6:
			if sys.argv[i] == "1":
				ssl = " -ssl"

	print "\n", sys.argv[0], "starting with:\n"
	print "\tServer: ", server
	print "\tPassword: ", password
	print "\tItem: ", item
	print "\tRepeat: ", repeat
	print "\tSomaclient: ", somaclient
	print
	
	list_cmd={}
	list_cmd["Read Directory"]= ("-rd", 0)
	list_cmd["Read Palinsesto"]=("-rp", 0)
	list_cmd["Read Spot"]=("-rs",0)
	list_cmd["FilePalinsesto"]=("-fp ./palinsesto.cfg_example",0)
	list_cmd["OldPalinsesto"]=("-op",0)
	list_cmd["GetPalinsesto"]=("-gp",0)
	list_cmd["GetOldPalinsesto"]=("-gop",0)
	list_cmd["SetDefaultPalinsesto"]=("-dp",0)
	list_cmd["FileSpot"]=("-fs ./spot.cfg_example",0)
	list_cmd["OldSpot"]=("-os",0)
	list_cmd["GetSpot"]=("-gs",1)
	list_cmd["GetOldSpot"]=("-gsp",0)
	list_cmd["SetDefaultSpot"]=("-ds",0)
	list_cmd["Status"]=("-st",0)
	list_cmd["NextItem"]=("-ns " + item,0)
	list_cmd["NextItemList"]=("-nsl",0)
	list_cmd["NextItemRemove"]=("-nsr",0)
	list_cmd["NextItemSetSpot"]=("-nsss 1",0)
	list_cmd["NextItemGetSpot"]=("-nsgs 1",1)
	list_cmd["Running"]=("-r",1)
	list_cmd["Skip"]=("-sk",0)
	list_cmd["Stop"]=("-1",0)
	list_cmd["Start"]=("-0",0)
	
	print "\n*** Single test... ***\n"
	for i in list_cmd.items():
		test(i[0], i[1][0], i[1][1])
	
	print "\n*** Recursive test... ***\n"
	for i in list_cmd.items():
		rec_test(i[0], i[1][0], i[1][1], list_cmd)
	
	print "If your somad are already running: be happy :)"
	

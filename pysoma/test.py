# Soma - Copyright (C) 2003-7 bakunin - Andrea Marchesini 
#                                       <bakunin@autistici.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
# 02110-1301, USA.
#
# This program is released under the GPL with the additional exemption that
# compiling, linking, and/or using OpenSSL is allowed.

import pysoma
import sys
from os import unlink
from time import sleep

try:
	k=pysoma.open_tcp("localhost", 12521, "passwd", 0);
except pysoma.error:
	print "connect failed!"
	sys.exit(1)

print "Ask status..."
print "STATUS:", k.status()
print "PALINSESTO:", k.palinsesto_name()
print "ITEM:", k.get_item()

print "PLAY TIME:"
for i in range(10):
	print "\t", k.get_time_play()
	sleep(1)

print "Read data..."
k.read_palinsesto()
k.read_directory()
k.read_spot()

print "Ask the palinsesto..."
data=k.get_palinsesto()
f=open("test.pysoma", "w")
f.write(data)
f.close()

print "Send a New palinsesto..."
k.new_palinsesto("test.pysoma")
unlink("test.pysoma")

print "Get the old palinsesto..."
data=k.get_old_palinsesto()

print "Set the old palinsesto..."
k.old_palinsesto()

print "Set the current palinsesto as default..."
k.set_default_palinsesto()

print "Ask the spot..."
data=k.get_spot()
f=open("test.pysoma", "w")
f.write(data)
f.close()

print "Send a New spot..."
k.new_spot("test.pysoma")
unlink("test.pysoma")

print "Get the old spot..."
data=k.get_old_spot()

print "Set the old spot..."
k.old_spot()

print "Set the current spot as default..."
k.set_default_spot()

print "Skip items:"
for i in range(10):
	sleep(1)
	print k.status()
	k.skip()

for i in range(10):
	print "stop"
	k.stop(1)  # if you want stop somad for 10 seconds: k.stop(10)
	sleep(1)
	print "start"
	k.start()
	sleep(1)

print "Distribuited Paths:"
for i in k.get_path("/"):
	print i
	if i[-1] == '/':
		for j in k.get_path("/" + i):
			if j[-1] == '/':
				data = k.get_stat_dir_path("/" + i + j)
				print "\t" + j + " ",
				print data["duration"]
	else:
		print i + "\t",
		print k.get_stat_path("/"+i)

print "Absolute Path:"
data=k.get_stat_dir("/home")
print "Duration of /home: ",
print data["duration"]

print "Current Item in transmission:"
for i in k.get_item_list():
	print i

print "Current Spot in transmission:"
for i in k.get_spot_list():
	print i

print "After this operations, somad is running?",
if k.running() == 1:
	print "yes"
else:
	print "no"

print "Remove Item:"
for i in range(10):
	k.remove_item(0)

	list = k.get_item_list()

	if list:
		for j in list:
			print j

	print "---"

print "Remove Spot:"
for i in range(10):
	k.remove_spot(0)

	list = k.get_spot_list()

	if list:
		for j in list:
			print j

	print "---"

print "On somad server the time is:", k.time()

print "bye :)"


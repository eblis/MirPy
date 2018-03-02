import os
import sys

if len(sys.argv) != 3:
	buffer = "Usage %s filename path_to_copy" % sys.argv[0]
	print buffer
	sys.exit()
	
(path, fileName) = os.path.split(sys.argv[1])
print "Cleaning up file '%s'" % os.path.normpath(fileName)  
fin = open(sys.argv[1], "rt")
newFile = os.path.join(sys.argv[2], fileName)
newFile = os.path.normpath(newFile)
fout = open(newFile, "wt")
print "The results will be copied to :", newFile
for line in fin.readlines():
	if not (line.find("#include \"Python.h\"") >= 0):
		fout.write(line)
fin.close()
fout.close()

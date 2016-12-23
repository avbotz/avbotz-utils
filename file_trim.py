#to run:
#python file_trim.py
#note: file must be in same folder

import os
import random



#returns whether or not a point in the photo will be written
def decision(probability):
    return random.random() > probability



print "Input filename:"
file = raw_input()
#print "input output filename"
#out_file = raw_input()



f = open(file,"r")
lines = f.readlines()
statinfo = os.stat(file)

#the numbers indicate the size the file will be cut down to
if statinfo.st_size > 10000:
   lines_delete = (float(statinfo.st_size-10000)/(float(statinfo.st_size)/float(len(lines))))/len(lines)
else:
   lines_delete = 0

f.close()



f = open(file,"w")
#f = open(out_file, "w")
for x in range(len(lines)):
    dec = decision(lines_delete)
    if dec == True:
        f.write(lines[x])
f.close()


#print os.stat(file).st_size

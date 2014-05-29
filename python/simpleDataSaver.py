#!/usr/bin/python
'''
    Command line tool to save the data from arduino to a file somewhere in a
    *nix based system...could work on windows, but has never been tested.

    Usage:
        invoke in commandline like this:
          "python2.7 simpleDataSaver.py '/dev/arduinoPORT' '/destination/folder/for/datafile' 
        asks for the subject ID,should be a integer number, and starts to save everything on
        ssID.log in the selected folder.
        To end acquisition, press ctrl+c, this will send the end sampling command to arduino, 
        and will close all open files and folders.
        ma python2.7 guarData.py '/dev/arduino'
'''

import time
import sys
import datetime
import serial

sjt=int(input('Subjects ID? (only integer numbers): \n'))
ard = str(sys.argv[1])
folder = str(sys.argv[2])
ser=serial.Serial(ard,9600)
time.sleep(3)
ser.write('C')
print('Starting...')
def writeVal(val):
  now=datetime.datetime.now()
  with  open(folder+str(sjt)+".log","a+") as f:
    f.write(val+"\t"+now.strftime("%Y-%m-%d    %H:%M:%S"))
    f.write("\n")
    f.close()



while True:
    try:
		if ser.inWaiting()>0:
			val=ser.readline()
			val=val.strip('\r\n')
			valb=val.split('\t')
            writeVal(val)
    except KeyboardInterrupt:
      print 'exiting'
      ser.write('F')
      ser.flush()
      ser.close()
      break

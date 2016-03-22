'''
	Copyright (C) 2016  Arthur Pachachura

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA
'''


import threading
from subprocess import Popen, PIPE
import os

#Touch Events:
# PRESSED = 1
# RELEASED = 2

class StoppableThread(threading.Thread):
        """Thread class with a stop() method. The thread itself has to check regularly for the stopped() condition."""
        def __init__(self):
                super(StoppableThread, self).__init__()
                self._stop = threading.Event()

        def stop(self):
                self._stop.set()

        def stopped(self):
                return self._stop.isSet()

class TouchThread (StoppableThread):
	_width = 480
	_height = 320
	_device = "/dev/input/event0"
	_state = (0, 0, 0)
	_prevstate = (0, 0, 0)
	_new = False
	_lock = threading.Lock()
	_init = False
	_error = None
	_wait = True
	_debug = False	

	def __init__(self, width=480, height=320, device="/dev/input/event0", debug=False):
		self._width = width
		self._height = height
		self._device = device
		self._debug = debug
		super(TouchThread, self).__init__()

	def isInitialized(self):
		return self._init

	def hasError(self):
		return (self._error is not None)

	def getError(self):
		return self._error

	def hasUpdate(self):
		self._lock.acquire()
		new = self._new
		self._lock.release()
		return new

	def getState(self):
		self._lock.acquire()
		state = self._state;
		if (self._wait is False):
			state = self._state
			self._state = (self._state[0], self._state[1], 0)
			self._new = False
		else:
			state = self._prevstate
		self._lock.release()
		return state
		
	def run(self):
		dir = os.path.dirname(os.path.realpath(__file__))
		process = Popen([dir + "/pytouch", str(self._width), str(self._height), str(self._device), "2"], stdout=PIPE, bufsize=1)
		if self._debug is True:
			print "STARTING TOUCH THREAD"
		for line in iter(process.stdout.readline, b''):
    			if (self._debug is True) and (not line.startswith("KEEPALIVE")):
				print line[:-1]
			if (self.stopped() is True):
				if self._debug is True:
					print "EXITING TOUCH THREAD by stop()"
				return;
			self._lock.acquire()
			if (line.startswith("KEEPALIVE")):
                                pass;
			elif (line.startswith("INIT: ")):
				print "GOT INIT!"
				self._init = True
			elif (line.startswith("KEY: ")):
				if (line[5:].startswith("DOWN")):
					self._prevstate = self._state
					self._state = (self._state[0], self._state[1], 1)
					self._wait = True
				elif (line[5:].startswith("UP")):
					self._state = (self._state[0], self._state[1], self._state[2] | 2)
			elif (line.startswith("TOUCH: ")):
				self._new = True
				self._wait = False
				pos = line[7:].index(" ")
				self._state = (int(line[7:pos+7]),int(line[pos+8:-1]), self._state[2])
			elif (line.startswith("ERROR: ")):
				self._error = line[7:-1]
				if self._debug is not True:
					print line
				self._lock.release()
				if self._debug is True:
					print "EXITING TOUCH THREAD with error"
				return;
			self._lock.release()
		
		process.communicate() # wait for process to exit
		if self._debug is True:
			print "EXITING TOUCH THREAD - process ended"

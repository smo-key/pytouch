# pytouch
Python Touchscreen Wrapper for Unix-based systems (works on Raspberry Pi!)

## Installation

To install the library, first navigate into its directory and run `build.sh` or `gcc -Wall -lpthread pytouch.c -o pytouch`, which will compile the C-side source to a binary.

## Usage

To import from a folder inside your current project, add the following lines to your Python code:
``` python
from PyTouch import pytouch
from PyTouch.pytouch import TouchThread as TouchThread
```

Touchscreen events are received asynchronously by the `TouchThread`. To start the thread without any parameters, run:
``` python
touch = TouchThread()
touch.start()
```

## API
``` python
TouchThread(width=480, height=320, dev="/dev/input/event0", debug=False)
```
Initialize the thread, returning a `threading.Thread` instance. `width` and `height` paramters indicate the size of the screen in pixels, or the size of your program's drawing surface if one exists. `dev` is the touchscreen device, typically `/dev/input/eventX` or `/dev/input/touchscreen`. If `debug == True`, messages from the C program will be directly printed to `stdout`.

``` python
TouchThread.getState()
```
Returns a tuple of the format `(touchPositionX, touchPositionY, touchState)`, where `touchPositionX` and `touchPositionY` are the last event's screen coordinates and `touchState` is a flag indicating whether the screen is currently `PRESSED` (1) or `RELEASED` (2).

The `touchState` parameter is reset to 0 when `getState()` is called - thus `getState()` acts like a queue that retrieves that last known position for a press. `touchState` may also be neither `PRESSED` nor `RELEASED` (0) or `PRESSED` and `RELEASED` (3). If both `PRESSED` and `RELEASED`, then a finger was both pressed and released in the time since the last call to `getState()`. Note that if taps are too rapid, a tap may be missed if your program does not call `getState()` frequently.

``` python
TouchThread.hasUpdate()
```
Returns `True` if the touchscreen has an touch update, `False` otherwise.

``` python
TouchThread.inInitialized()
```
Returns `True` if the touchscreen is ready to receive events, `False` otherwise.

``` python
TouchThread.stop()
```
The thread can be killed by running `touch.stop()` (thread will exit within approximately 1 second).

``` python
TouchThread.stopped()
```
Returns `True` if the thread is exiting, `False` otherwise. Thread may not have completely exited, use `TouchThread.join()` to wait for the thread to finish.

## License

Copyright (C) 2016  Arthur Pachachura

Copyright (C) 2013  Mark Williams

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

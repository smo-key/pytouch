/*
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
*/


#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "touch.h"
#include "touch.c"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SAMPLE_AMOUNT 1
#define DEFAULT_SCREEN_WIDTH 480
#define DEFAULT_SCREEN_HEIGHT 320

pthread_t thread = NULL;

void  INThandler(int sig)
{
        signal(sig, SIG_IGN);
	pthread_cancel(thread);
        exit(0);
}

void* keepaliveThread(void *arg)
{
	printf("TIMER STARTED\n");
	int* t_ptr = (int *)arg;
	while(1)
	{
		sleep(*t_ptr);
		printf("KEEPALIVE\n");
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, (char *) NULL, _IOLBF, 0); /* make line buffered stdout */
	signal(SIGINT, INThandler);

	int xres, yres;

	int Xsamples[20];
	int Ysamples[20];

	int screenXmax, screenXmin;
	int screenYmax, screenYmin;

	int rawX, rawY, rawPressure, scaledX, scaledY;

	int Xaverage = 0;
	int Yaverage = 0;
	int keepalive = 0;

	char* tsname = "/dev/input/event0";

	if (argc < 3)
	{
		xres = DEFAULT_SCREEN_WIDTH;
		yres = DEFAULT_SCREEN_HEIGHT;
	}
	else
	{
		xres = atoi(argv[1]);
		yres = atoi(argv[2]);
	}
	if (argc >= 4)
	{
		tsname = argv[3];
	}
	if (argc >= 5)
	{
		keepalive = atoi(argv[4]);
	}

	if (openTouchScreen(tsname) == 1)
	{
		printf("ERROR: Error reading touchscreen.\n");
		return 1;
	}

	getTouchScreenDetails(&screenXmin,&screenXmax,&screenYmin,&screenYmax);

	if (keepalive > 0)
	{
		if (pthread_create(&thread, NULL, keepaliveThread, &keepalive))
		{
			printf("ERROR: Cannot create keepalive thread");
			return 1;
		}
	}

	//scaleXvalue = ((float)screenXmax-screenXmin) / xres;
	//scaleYvalue = ((float)screenYmax-screenYmin) / yres;

	int sample, x;

	printf("INIT: SUCCESS!\n");

	while(1){
		for (sample = 0; sample < SAMPLE_AMOUNT; sample++){
			getTouchSample(&rawX, &rawY, &rawPressure);
			Xsamples[sample] = rawX;
			Ysamples[sample] = rawY;
		}

		Xaverage  = 0;
		Yaverage  = 0;

		for ( x = 0; x < SAMPLE_AMOUNT; x++ ){
			Xaverage += Xsamples[x];
			Yaverage += Ysamples[x];
		}

		Xaverage = Xaverage/SAMPLE_AMOUNT;
		Yaverage = Yaverage/SAMPLE_AMOUNT;

		scaledX = (int)(((float)Xaverage - (float)screenXmin)/((float)screenXmax-(float)screenXmin)*((float)xres));
		scaledY = (int)(((float)Yaverage - (float)screenYmin)/((float)screenYmax-(float)screenYmin)*((float)yres));
		printf ("TOUCH: %i %i\n", scaledX, scaledY);
	}
}



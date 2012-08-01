/*
	WIP
	Code based on an example code by https://github.com/gpisacco
	All modifications released under CC0 http://creativecommons.org/publicdomain/zero/1.0/ meaning public domain.
	Toni Lähdekorpi <toni@lygon.net>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

int main(int argc, char* argv[]) {
	struct input_event ev[64];
	int fevdev = -1;
	int result = 0;
	int size = sizeof(struct input_event);
	int rd;
	int value;
	char name[256] = "Unknown";
	char *device = "/dev/input/by-id/usb-NewRoad_Sem._NewRoad_System_PS2_Interface-event-mouse"; // Our HID RFID reader, example: 0419:0912 Samsung Info. Systems America, Inc.
	char code[128], command[128];


	keycodes = {
[0]="UNKNOWN",
[1]="ESCAPE",        [2]="1",  [3]="2",  [4]="3",  [5]="4",  [6]="5",
 [7]="6",  [8]="7",  [9]="8", [10]="9",
[11]="0", [12]="-", [13]="=", [14]="BACKSPACE",   [15]="TAB",
[16]="q", [17]="w", [18]="e", [19]="r", [20]="t",
[21]="y", [22]="u", [23]="i", [24]="o", [25]="p",
[26]="[", [27]="]", [28]="ENTER",       [29]="LEFTCTRL",    [30]="a",
[31]="s", [32]="d", [33]="f", [34]="g", [35]="h",
[36]="j", [37]="k", [38]="l", [39]=";", [40]="'",
[41]="~", [42]="LEFTSHIFT",   [43]="\\",[44]="z", [45]="x",
[46]="c", [47]="v", [48]="b", [49]="n", [50]="m",
[51]=",", [52]=".", [53]="/", [54]="RIGHTSHIFT",  [55]="*",
[56]="LEFTALT",     [57]=" ", [58]="CAPSLOCK",    [59]="F1",[60]="F2",
[61]="F3",[62]="F4",[63]="F5",[64]="F6",[65]="F7",
[66]="F8",[67]="F9",[68]="F10",[69]="NUMLOCK",    [70]="SCROLLLOCK",
[71]="7", [72]="8", [73]="9", [74]="-", [75]="4",
[76]="5", [77]="6", [78]="+", [79]="1", [80]="2",
[81]="3", [82]="0", [83]=".", 
[87]="F11",[88]="F12",
[156]="\n",[157]="RIGHTCTRL",[181]="/",[184]="RIGHTALT",   [199]="HOME",
[200]="ARROW_UP",  [201]="PAGEUP",     [203]="ARROW_LEFT", [205]="ARROW_RIGHT",
[207]="END",       [208]="ARROW_DOWN", [209]="PAGEDOWN",   [210]="INSERT",
[211]="DELETE",    [256]="LEFTMOUSE",  [257]="RIGHTMOUSE", [258]="MIDDLEMOUSE",
[259]="MOUSE3",    [260]="MOUSE4",     [261]="MOUSE5",     [262]="MOUSE6",
[263]="MOUSE7",    [264]="MOUSEWHEELUP",[265]="MOUSEWHEELDOWN",
}


	pid_t pid;
   
	pid = fork();
	if (pid < 0) {
		printf("Error forking process. Could not create a daemon.\n");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		printf("Our pid: %d",pid);
		exit(EXIT_SUCCESS);
	}

	fevdev = open(device, O_RDONLY); // We open our USB HID as read only
	if (fevdev == -1) {
		printf("Failed to open RFID reader.\n");
		exit(1);
	}

	result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
	printf ("Reading From : %s (%s)\n", device, name);

	printf("Getting exclusive access: ");
	result = ioctl(fevdev, EVIOCGRAB, 1);
	printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");

	while (1) {
		if ((rd = read(fevdev, ev, size * 64)) < size) {
			break;
		}
		value = ev[0].value;
		if (value != ' ' && ev[1].value == 1 && ev[1].type == 1) {
			if(ev[1].code == 42) {
			printf("Received 42, RFID starting:\n");
		} else {
			sprintf(code + strlen(code),"%d;",(ev[1].code));
			printf ("%d;", (ev[1].code));
			if(ev[1].code == 28){
			printf("\nRFID ended\n");
			printf("[%s] is a string\n",code);
			sprintf(command,"./action \"%s\"",code);
			system(command);
			*(code) = 0;
			*(command) = 0;
		}
	}
	printf("Exiting.\n");
	result = ioctl(fevdev, EVIOCGRAB, 1);
	close(fevdev);
	return 0;
}
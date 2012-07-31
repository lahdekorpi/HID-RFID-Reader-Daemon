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

int main(int argc, char* argv[])
{
    struct input_event ev[64];
    int fevdev = -1;
    int result = 0;
    int size = sizeof(struct input_event);
    int rd;
    int value;
    char name[256] = "Unknown";
    char *device = "/dev/input/by-id/usb-NewRoad_Sem._NewRoad_System_PS2_Interface-event-mouse"; // Our HID RFID reader, example: 0419:0912 Samsung Info. Systems America, Inc.
    char code[128], command[128];

	pid_t pid;
   
    pid = fork();
    if (pid < 0) {
        print("Error forking process. Could not create a daemon.\n");
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

    while (true)
    {
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
			 printf ("[%s] is a string\n",code);
			sprintf(command,"./action \"%s\"",code);
			system(command);
			*(code) = 0;
			*(command) = 0;
	}
		}
	}
    }
    printf("Exiting.\n");
    result = ioctl(fevdev, EVIOCGRAB, 1);
    close(fevdev);
    return 0;
}

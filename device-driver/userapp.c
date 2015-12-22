/*
 *  User application example, which interacts with the char-device. 
 *  You need to install the char-device and create the device path first.
 *  See char-device.c file top comments.
 * 
 *  Build with:
 *  # gcc -o userapp userapp.c
 *
 *  Give access to device file or run as root (device file was created 
 *  through char-device, see instructions in char-device.c):
 *  # sudo chmod 777 /dev/mychardevice
 *
 *  View log:
 *  # dmesg
 *
 *  Tested on Ubuntu 12.04
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/mychardevice"

int main()
{
	int len, fd;
	char ch, write_buf[100], read_buf[100]; 

	fd = open(DEVICE, O_RDWR); 

	if(fd == -1)
	{
		printf("file %s does not exist or has been already locked by another process.\n", DEVICE);
		exit(-1);
	}

	printf("r = read from device\nw = write to device\nenter command: ");
	scanf("%c", & ch);

	switch(ch)
	{
		case 'w':
			printf("enter data: ");
			scanf("%s", write_buf);
			write(fd, write_buf, sizeof(write_buf));
			break;

		case 'r':
			read(fd, read_buf, sizeof(read_buf));
			printf("device: %s\n", read_buf);
			break;

		default:
			printf("Command not recognized\n");
			break;
	}
	close(fd);

	return 0;
}
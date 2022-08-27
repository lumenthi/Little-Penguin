#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void main(int argc, char **argv)
{
	char buf[50];
	int fd = open("/dev/fortytwo", O_RDWR);
	if (fd == -1)
		return;
	int i = 0;
	int ret = 0;
	while (i < 10) {
		bzero(&buf, sizeof(buf));
		ret = read(fd, buf, 1);
		i++;
		printf("%s, ret: %d\n", buf, ret);
	}
	close(fd);
	return;
}

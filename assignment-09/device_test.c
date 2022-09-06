#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void main(int argc, char **argv)
{
	char buf[1];
	int fd = open("/proc/mymounts", O_RDWR);
	if (fd == -1)
		return;
	/* read test */
	int i = 0;
	int ret = 0;
	while (i < 500) {
		bzero(&buf, sizeof(buf));
		ret = read(fd, buf, 1);
		i++;
		printf("%s, ret: %d\n", buf, ret);
	}
	close(fd);
	return;
}

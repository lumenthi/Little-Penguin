#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void id_test()
{
	printf("==========ID=============\n");
	char buf[50];
	int fd = open("/sys/kernel/debug/fortytwo/id", O_RDWR);
	if (fd == -1)
		return;
	/* read test */
	int i = 0;
	int ret = 0;
	while (i < 9) {
		bzero(&buf, sizeof(buf));
		ret = read(fd, buf, 1);
		i++;
		printf("%s, ret: %d\n", buf, ret);
	}
	/* write test */
	ret = write(fd, "lumenthi", 8);
	printf("=valid=\nwrite ret: %d\n", ret);
	ret = write(fd, "invalid", 7);
	printf("=invalid=\nwrite ret: %d\n", ret);
	close(fd);
}

void jiffies_test()
{
	printf("==========JIFFIES=============\n");
	char buf[50];
	int fd = open("/sys/kernel/debug/fortytwo/jiffies", O_RDWR);
	if (fd == -1)
		return;
	/* read test */
	int i = 0;
	int ret = 0;
	while (i < 10) {
		bzero(&buf, sizeof(buf));
		ret = read(fd, buf, 1);
		i++;
		printf("%s, ret: %d\n", buf, ret);
	}
	/* write test */
	ret = write(fd, "not possible", 12);
	printf("=invalid=\nwrite ret: %d\n", ret);
	close(fd);
}

void foo_test()
{
	printf("==========FOO=============\n");
	char buf[1];
	int fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR);
	int i = 0;
	int ret = 0;
	int current_pid = getuid();
	int dsize = 8192;
	char dbuf[dsize];

	if (fd == -1)
		return;

	memset(dbuf, 'a', dsize);

	/* write test */
	setuid(0); /* root */
	ret = write(fd, dbuf, dsize);
	printf("wrote %d bytes\n", ret);

	/* read test */
	ret = 0;
	setuid(current_pid);
	while (i < dsize) {
		bzero(&buf, sizeof(buf));
		ret += read(fd, buf, 1);
		i++;
	}
	printf("read %d bytes\n", ret);
	close(fd);
}

void main(int argc, char **argv)
{
	id_test();
	jiffies_test();
	foo_test();
	return;
}

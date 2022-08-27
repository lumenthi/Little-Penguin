# Read device driver
sudo cat /dev/fortytwo
# Write to the device driver
sudo sh -c 'echo -n "lumenthi" > /dev/fortytwo'

# Return of read or write can be:
- The number of bytes transferred
- 0 to mark the end of the file
- Negative value indicating an error code

===============READ================
# Code
static ssize_t misc_read(
						struct file *file,
						char __user *user_buffer,
						size_t size,
						loff_t *offset # Cursor in the file
						)

## Prevent read overflow
size = size > login_len ? login_len: size;

## Return 0 if the cursor is past the login
if (*offset >= login_len)

## Copy size of the login (starting from offset) to the user_buffer
## ret contains the amount of char that wasnt successfully written
ret = copy_to_user(user_buffer, login+*offset, size)

## Increment the cursor
*offset += size-ret;

## Return number of read characters
return size-ret;

# Read test
$ sudo ./a.out
l, ret: 1
u, ret: 1
m, ret: 1
e, ret: 1
n, ret: 1
t, ret: 1
h, ret: 1
i, ret: 1

$ sudo cat /dev/fortytwo
lumenthi%

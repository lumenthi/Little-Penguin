# Description:
## When writing:
	The module writes the content of user buffer into global str[PAGE_SIZE]
## When reading
	The module kmalloc a tmp buffer to perform its reverse operation
	Then writes the content of this tmp buffer into the user's buffer
	It frees the tmp buffer then returns the number of bytes wrotten

# After fixing the module
$ make load

$ sudo sh -c 'echo -n "lumenthi" > /dev/reverse' && sudo cat /dev/reverse
ihtnemul

$ sudo sh -c 'echo -n "ihtnemul" > /dev/reverse' && sudo cat /dev/reverse
lumenthi%

# Kernel style checking
$ ./scripts/checkpatch.pl -f ../little-penguin/assignment-08/main.c
total: 0 errors, 0 warnings, 86 lines checked

../little-penguin/assignment-08/main.c has no obvious style problems and is ready for submission.

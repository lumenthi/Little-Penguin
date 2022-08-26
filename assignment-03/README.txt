# Linux provides a script to check Linux Coding Style convention: scripts/checkpatch.pl
# Be sure to run it from the kernel repo
➜  assignment-03 git:(master) ✗ ./checkpatch.pl -f main.c
Must be run from the top-level dir. of a kernel tree

==========OUTPUT============
➜  linux-5.11 ./scripts/checkpatch.pl -f ../little-penguin/assignment-03/main.c 
WARNING: Missing or malformed SPDX-License-Identifier tag in line 1
#1: FILE: ../little-penguin/assignment-03/main.c:1:
+#include <linux/module.h>

WARNING: Missing a blank line after declarations
#11: FILE: ../little-penguin/assignment-03/main.c:11:
+	int z;
+	for (x = 0; x < my_int; ++x)

ERROR: that open brace { should be on the previous line
#11: FILE: ../little-penguin/assignment-03/main.c:11:
+	for (x = 0; x < my_int; ++x)
+	{

WARNING: Missing a blank line after declarations
#26: FILE: ../little-penguin/assignment-03/main.c:26:
+	int x = 10;
+	x = do_work(&x, x);

total: 1 errors, 3 warnings, 34 lines checked
[...]
=====================================================================
After fixes:
➜  linux-5.11 ./scripts/checkpatch.pl -f ../little-penguin/assignment-03/main.c
WARNING: Missing or malformed SPDX-License-Identifier tag in line 1
#1: FILE: ../little-penguin/assignment-03/main.c:1:
+#include <linux/module.h>


# About SPDX-License:
Placement:
The SPDX license identifier in kernel files shall be added at the first possible line in a file which can contain a comment. For the majority of files this is the first line, except for scripts which require the ‘#!PATH_TO_INTERPRETER’ in the first line. For those scripts the SPDX identifier goes into the second line.

Style:
The SPDX license identifier is added in form of a comment. The comment style depends on the file type:
C source: // SPDX-License-Identifier: <SPDX License Expression>
C header: /* SPDX-License-Identifier: <SPDX License Expression> */
ASM:      /* SPDX-License-Identifier: <SPDX License Expression> */
scripts:  # SPDX-License-Identifier: <SPDX License Expression>
.rst:     .. SPDX-License-Identifier: <SPDX License Expression>
.dts{i}:  // SPDX-License-Identifier: <SPDX License Expression>

# Add this a the very beginning of the file:
// SPDX-License-Identifier: GPL-2.0+

======================================================================
➜  linux-5.11 ./scripts/checkpatch.pl -f ../little-penguin/assignment-03/main.c
total: 0 errors, 0 warnings, 36 lines checked

../little-penguin/assignment-03/main.c has no obvious style problems and is ready for submission.
=======================================================================
Remove useless return (return 1)
Remove unused param retval

# Opening Subprocess

There are many ways how to open the process in posix based operation system. In my case I need `process id` of the child process. The function `system` can be use to open the process. It returns 0 when successful and it redirects output and function is blocking. Retrieving of `process id` is not steightforward. The function `popen` can be used, it gives pipes for standard input and output to the child process, but there is a struggle to find `process id` of the child process. Finally, there is a combination of functions `fork` and `exec`. Fork basically duplicates current process and gives child's `process id` to the parent. Then exec is used to replace new process by starting new program. Fork and exec is low level underline implementation of `popen`.

In this preparation project the brand new function `system2` was crafted and tested. The new function hits all the boxes. It provides output pipes like `popen`, is nonblocking and and provides `process id` to the parent.


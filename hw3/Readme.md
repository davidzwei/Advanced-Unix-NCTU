# HW3 Extend the Mini Lib C to Handle Signals
In this homework, you have to extend the mini C library introduced in the class to support signal relevant system calls. You have to implement the following C library functions in Assembly and C using the syntax supported by yasm x86_64 assembler.

- setjmp: prepare for long jump by saving the current CPU state. In addition, preserve the signal mask of the current process.
- longjmp: perform the long jump by restoring a saved CPU state. In addition, restore the preserved signal mask.
- signal and sigaction: setup the handler of a signal.
- sigprocmask: can be used to block/unblock signals, and get/set the current signal mask.
- sigpending: check if there is any pending signal.
- alarm: setup a timer for the current process.
- write: write to a file descriptor.
- pause: wait for signal
- sleep: sleep for a specified number of seconds
- exit: cause normal process termination
- strlen: calculate the length of the string, excluding the terminating null byte ('\0').
- functions to handle sigset_t data type: sigemptyset, sigfillset, sigaddset, sigdelset, and sigismember.

## Usage
```
$ make
```



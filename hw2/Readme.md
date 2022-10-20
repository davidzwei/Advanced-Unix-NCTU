# Monitor File Activities of Dynamically Linked Programs
In this homework, we aim to practice library injection and API hijacking. Please implement a simple logger program that can show file-access-related activities of an arbitrary binary running on a Linux operating system. You have to implement your logger in two parts. One is a logger program that prepares the runtime environment to inject, load, and execute a monitored binary program. The other is a shared object that can be injected into a program by the logger using LD_PRELOAD. You have to dump the library calls as well as the passed parameters and the returned values. Please check the "Requirements" section for more details.

## Usage
```
$ make
```
```
usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]
    -p: set the path to logger.so, default = ./logger.so
    -o: print output to file, print to "stderr" if no file specified
    --: separate the arguments for logger and for the command
```

```
$ ./logger ./sample
[logger] creat("/home/ta/hw2/tmp/aaaa", 600) = 3
[logger] chmod("/home/ta/hw2/tmp/aaaa", 666) = 0
[logger] chown("/home/ta/hw2/tmp/aaaa", 65534, 65534) = -1
[logger] rename("/home/ta/hw2/tmp/aaaa", "/home/ta/hw2/tmp/bbbb") = 0
[logger] open("/home/ta/hw2/tmp/bbbb", 1101, 666) = 4
[logger] write("/home/ta/hw2/tmp/bbbb", "cccc.", 5) = 5
[logger] close("/home/ta/hw2/tmp/bbbb") = 0
[logger] open("/home/ta/hw2/tmp/bbbb", 000, 000) = 4
[logger] read("/home/ta/hw2/tmp/bbbb", "cccc.", 100) = 5
[logger] close("/home/ta/hw2/tmp/bbbb") = 0
[logger] tmpfile() = 0x55c418842670
[logger] fwrite("cccc.", 1, 5, "/tmp/#14027911 (deleted)") = 5
[logger] fclose("/tmp/#14027911 (deleted)") = 0
[logger] fopen("/home/ta/hw2/tmp/bbbb", "r") = 0x55c418842670
[logger] fread("cccc.", 1, 100, "/home/ta/hw2/tmp/bbbb") = 5
[logger] fclose("/home/ta/hw2/tmp/bbbb") = 0
[logger] remove("/home/ta/hw2/tmp/bbbb") = 0
sample done.
[logger] write("/dev/pts/19", "sample done...", 14) = 14
```

```
$ ./logger
no command given.

$ ./logger -h                
./logger: invalid option -- 'h'
usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]
        -p: set the path to logger.so, default = ./logger.so
        -o: print output to file, print to "stderr" if no file specified
        --: separate the arguments for logger and for the command

$ ./logger ls                
[logger] fopen("/proc/filesystems", "re") = 0x55aa12cd6ad0
[logger] fclose("/proc/filesystems") = 0
hw2.c  logger  logger.so  Makefile  sample  sample.c  sample.txt
[logger] fclose("/dev/pts/7") = 0
[logger] fclose("/dev/pts/7") = 0

$ ./logger ls 2>/dev/null 
hw2.c  logger  logger.so  Makefile  sample  sample.c  sample.txt

$ ./logger -o ls_al.txt -- ls -al
... [output of `ls -al`] ...

$ cat ls_al.txt       
[logger] fopen("/proc/filesystems", "re") = 0x558c872fcad0
[logger] fclose("/proc/filesystems") = 0
[logger] fopen("/etc/passwd", "rme") = 0x558c872fcd00
[logger] fclose("/etc/passwd") = 0
[logger] fopen("/etc/group", "rme") = 0x558c872fcf30
[logger] fclose("/etc/group") = 0
[logger] fclose("/dev/pts/7") = 0
[logger] fclose("/dev/pts/7") = 0
```

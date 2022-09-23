#define _GNU_SOURCE
#include <iostream>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){   
    char* cmd = NULL;
    char** cmd_arg = NULL;
    bool is_path = 0;
    bool is_file = 0;
    int opt = 0;
    string out_file, so_path;
    
    if(argc == 1){
        cout <<"no command given.\n";
        exit(1);
    }
    while((opt = getopt(argc, argv, "o:p:")) != -1)
    {
        switch(opt)
        {
            case 'o':
                is_file = 1;
                out_file = optarg;
                break;
            case 'p':
                is_path = 1;
                so_path = optarg;
                break;
            default:
                cout << "usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]\n";
                cout << "\t-p: set the path to logger.so, default = ./logger.so\n";
                cout << "\t-o: print output to file, print to \"stderr\" if no file specified\n";
                cout << "\t--: separate the arguments for logger and for the command\n";
                exit(1);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "no command given.\n");
        return 0;
    }
    // default
    if(!is_path)
        so_path = "./logger.so";
    if(!is_file){
        setenv("OUTPUT_FILE", "STDERR", 1);
    }
    else{
        setenv("OUTPUT_FILE", out_file.c_str(), 1);
        close(open(out_file.c_str(), O_CREAT | O_TRUNC, 0644));
    }
    setenv("LD_PRELOAD", so_path.c_str(), 1);    
    cmd = argv[optind];
    cmd_arg = argv + optind;
    execvp(cmd, cmd_arg);
}
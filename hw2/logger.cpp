#define _GNU_SOURCE

#include <iostream>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
#define LEN             1024
#define PRINT_SIZE      32
#define LOGGER          "[logger]"

typedef int     (*o_chmod)(const char * path, mode_t mode);
typedef int     (*o_chown)(const char *pathname, uid_t owner, gid_t group);
typedef int     (*o_close)(int fd);
typedef int     (*o_creat)(const char *pathname, mode_t mode);
typedef int     (*o_creat64)(const char *pathname, mode_t mode);
typedef int     (*o_fclose)(FILE *stream);
typedef FILE*   (*o_fopen)(const char *filename, const char * mode);
typedef FILE*   (*o_fopen64)(const char *filename, const char * mode);
typedef size_t  (*o_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
typedef size_t  (*o_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
typedef int     (*o_open)(const char *pathname, int flags, ...);
typedef int     (*o_open64)(const char *pathname, int flags, ...);
typedef ssize_t (*o_read)(int fd, void *buf, size_t count);
typedef int     (*o_remove)(const char *pathname);
typedef int     (*o_rename)(const char *oldpath, const char *newpath);
typedef FILE*   (*o_tmpfile)(void);
typedef FILE*   (*o_tmpfile64)(void);
typedef ssize_t (*o_write)(int fd, const void *buf, size_t count);

int fd_flag = 0; //1:specifed file
int stderr_fd = 2; // which file to print
FILE* out_file = NULL;

void init_fd(){   
    o_open old_open = (o_open)dlsym(RTLD_NEXT, "open");
    o_write old_write = (o_write)dlsym(RTLD_NEXT, "write");
    // specified file
    if (strncmp(getenv("OUTPUT_FILE"), "STDERR", 6) != 0) {
        stderr_fd = old_open(getenv("OUTPUT_FILE"), O_RDWR | O_CREAT | O_APPEND, 0644);
    }
    fd_flag = 1;
}

void print(string str){
    if(!fd_flag)
        init_fd();
    o_write old_write = (o_write)dlsym(RTLD_NEXT, "write");
    old_write(stderr_fd, str.c_str(), strlen(str.c_str()));
}

void get_real_path(const char* path, char real_path[]){
    char* exist;
    exist = realpath(path, real_path);
    if (exist == NULL) {
        strcpy(real_path, path);
    }
}

void fd2name(int fd, char*buf){
    char path[LEN];
    memset(path, 0, LEN);
    sprintf(path, "/proc/self/fd/%d", fd);
    int result = readlink(path, buf, LEN);
}

int chmod(const char * path, mode_t mode){
    o_chmod o_func = (o_chmod)dlsym(RTLD_NEXT, "chmod");
    int ret = o_func(path, mode);

    char real_path[100] = {'\0'};
    get_real_path(path, real_path);    
    char mode_str[10];
    sprintf(mode_str, "%o", mode);
    
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %s) = %d\n", LOGGER, __func__, real_path, mode_str, ret);
    print(string(result));
    return ret;
}

int chown(const char *path, uid_t owner, gid_t group){
    o_chown o_func = (o_chown)dlsym(RTLD_NEXT, "chown");    
    int ret = o_func(path, owner, group);

    char real_path[100] = {'\0'};
    get_real_path(path, real_path);
    
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %d, %d) = %d\n", LOGGER, __func__, real_path, owner, group, ret);
    print(string(result));
    return ret;
}

int close(int fd){
    if(!fd_flag)
        init_fd();
    o_close o_func = (o_close)dlsym(RTLD_NEXT, "close");
    if(fd == stderr_fd){
        stderr_fd = dup(stderr_fd);
    }
    string str_fd = to_string(fd), path;
    char buf[1024];
    int len = readlink(string("/proc/self/fd/" + str_fd).c_str(), buf, 1023);
    if(len != -1){
        buf[len] = '\0';
        path = string(buf);
    }
    int ret = o_func(fd);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\") = %d\n", LOGGER, __func__, path.c_str(), ret);
    print(string(result));
    return ret;
}

int creat(const char *path, mode_t mode){
    o_creat o_func = (o_creat)dlsym(RTLD_NEXT, "creat");
    int ret = o_func(path, mode);
    char real_path[100] = {'\0'};
    get_real_path(path, real_path);
    char mode_str[10];
    sprintf(mode_str, "%o", mode);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %s) = %d\n", LOGGER, __func__, real_path, mode_str, ret);
    print(string(result));
    return ret;
}

int creat64(const char *path, mode_t mode){
    o_creat64 o_func = (o_creat64)dlsym(RTLD_NEXT, "creat64");
    int ret = o_func(path, mode);
    char real_path[100] = {'\0'};
    get_real_path(path, real_path);
    char mode_str[10];
    sprintf(mode_str, "%o", mode);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %s) = %d\n", LOGGER, __func__, real_path, mode_str, ret);
    print(string(result));
    return ret;
}

int fclose(FILE *stream){
    if(!fd_flag)
        init_fd();
    o_fclose o_func = (o_fclose)dlsym(RTLD_NEXT, "fclose");
    int fd = fileno(stream);
    if(fd == stderr_fd)
        stderr_fd = dup(stderr_fd);
    char buf[LEN];
    string path;
    int len = readlink(string("/proc/self/fd/" + to_string(fd)).c_str(), buf, LEN);
    if(len != -1){
        buf[len] = '\0';
        path = string(buf);
    }
    int ret = o_func(stream);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\") = %d\n", LOGGER, __func__, path.c_str(), ret);
    print(result);
    return ret;
}

FILE* fopen(const char *filename, const char * mode){
    o_fopen o_func = (o_fopen)dlsym(RTLD_NEXT, "fopen");
    FILE* ret = o_func(filename, mode);
    
    char real_path[100] = {'\0'};
    get_real_path(filename, real_path);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", \"%s\") = %p\n", LOGGER, __func__, real_path, mode, ret);
    print(string(result));
    return ret;
}

FILE* fopen64(const char *filename, const char * mode){
    o_fopen64 o_func = (o_fopen64)dlsym(RTLD_NEXT, "fopen64");
    // string to_print = "[logger] fopen64(";
    char real_path[100] = {'\0'};
    get_real_path(filename, real_path);
    
    // to_print += "\"" + string(mode) + "\") = ";
    FILE* ret = o_func(filename, mode);
    
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", \"%s\") = %p\n", LOGGER, __func__, real_path, mode, ret);
    print(string(result));
    return ret;
}

ssize_t read(int fd, void *buf, size_t count){
    o_read o_func = (o_read)dlsym(RTLD_NEXT, "read");
    ssize_t ret = o_func(fd, buf, count);

    char name[LEN];
    memset(name, 0, LEN);
    fd2name(fd, name);

    char result1[LEN] = {'\0'};
    string result2 = "";
    char result3[LEN] = {'\0'};
    sprintf(result1, "%s %s(\"%s\", \"", LOGGER, __func__, string(name).c_str());
    for(int i=0; i<count && i<PRINT_SIZE && i<ret; i++){
        if(isprint((int)*((char*)buf+i)) == 0)
            result2 += ".";
        else                                        //sprintf(result2, "%c", *((char*)name+i));
            result2 += *((char*)buf+i);
    }
    sprintf(result3, "\", %ld) = %ld\n", count, ret);
    string result = string(result1) + result2 + string(result3);
    print(result);
    return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    o_fread o_func = (o_fread)dlsym(RTLD_NEXT, "fread");
    size_t ret = o_func(ptr, size, nmemb, stream);
    int fd = fileno(stream);
    
    char name[LEN];
    memset(name, 0, LEN);
    char buf[1024] = {'\0'};
    int len = readlink(string("/proc/self/fd/" + to_string(fd)).c_str(), buf, 1023);

    char *p = (char*)ptr;
    char result1[LEN] = {'\0'};
    string result2 = "";
    char result3[LEN] = {'\0'};
    sprintf(result1, "%s %s(\"", LOGGER, __func__);
    for(int i=0; i<size*nmemb && i<PRINT_SIZE && i<ret; i++){
        if(isprint(p[i]) == 0)      result2 += ".";
        else                        result2 += p[i];
    }
    sprintf(result3, "\", %ld, %ld, \"%s\") = %ld\n", size, nmemb, string(buf).c_str(), ret);
    string result = string(result1) + result2 + string(result3);
    print(result);
    return ret;
}

ssize_t write(int fd, const void *buf, size_t count){
    o_write old_write = (o_write)dlsym(RTLD_NEXT, "write");
    // string to_print = "[logger] write(";
    string str_fd = to_string(fd), path;
    char buff[LEN];
    int len = readlink(string("/proc/self/fd/" + str_fd).c_str(), buff, 1023);
    if(len != -1){
        buff[len] = '\0';
        path = string(buff);
    }
    ssize_t ret = old_write(fd, buf, count);
    char result1[LEN] = {'\0'};
    char result2[32]  = {'\0'};
    char result3[LEN] = {'\0'};
    string path2 = "";

    char buff2[100];
    int size = 32;
    if(count < size)
        size = count;
    memcpy(buff2, buf, size);
    // clear type
    sprintf(result1, "%s %s(\"%s\", \"", LOGGER, __func__, path.c_str());
    for(int i=0; i<count && i<PRINT_SIZE && i<ret; i++){
        if(isprint((int)buff2[i]) == 0)     path2 += '.';
        else                                path2 += buff2[i];
    }
    sprintf(result3, "\", %ld) = %ld\n", count, ret);
    string result = string(result1) + path2 + string(result3);
    print(result);
    return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    o_fwrite o_func = (o_fwrite)dlsym(RTLD_NEXT, "fwrite");
    int fd = fileno(stream);
    char buf[1024] = {0};
    string path;
    int len = readlink(string("/proc/self/fd/" + to_string(fd)).c_str(), buf, 1023);
    if(len != -1){
        buf[len] = '\0';
        path = string(buf);
    }
    size_t ret = o_func(ptr, size, nmemb, stream);

    char buff[100];
    int new_size = 32;
    if(ret * size < new_size)
        new_size = ret * size;
    memcpy(buff, ptr, new_size);
    
    string path2 = "";
    char result1[LEN] = {'\0'};
    char result2[32]  = {'\0'};
    char result3[LEN] = {'\0'};

    sprintf(result1, "%s %s(\"", LOGGER, __func__);
    // sprintf(result1, "%s %s(%s, \"", LOGGER, __func__, name);
    for(int i=0; i<new_size && i<PRINT_SIZE && i<ret; i++){
        int temp = int(buff[i]);
        if(isprint(temp) == 0)      path2 += '.';
        else                        path2 += buff[i];
    }
    sprintf(result3, "\", %ld, %ld, \"%s\") = %ld\n", size, nmemb, path.c_str(), ret);
    string result = string(result1) + path2 + string(result3);
    print(result);
    return ret;
}

int open(const char *pathname, int flags, ...){
    mode_t mode = 0;
    o_open o_func = (o_open)dlsym(RTLD_NEXT, "open");
    
    int ret = 0;
    char long_path[100] = {0}, mode_str[10] = {0}, flag_str[10] = {0};
    char real_path[LEN] = {'\0'};
    get_real_path(pathname, real_path);
    sprintf(flag_str, "%o", flags);
        
    if(__OPEN_NEEDS_MODE (flags)){
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
        sprintf(mode_str, "%o", mode);
        ret = o_func(pathname, flags, mode);
    }
    else{
        ret = o_func(pathname, flags);
        sprintf(mode_str, "%o", mode);
    }
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %s, %s) = %d\n", LOGGER, __func__, string(real_path).c_str(), flag_str, mode_str, ret);
    print(string(result));
    return ret;
}

int open64(const char *pathname, int flags, ...){
    mode_t mode = 0;
    o_open64 o_func = (o_open64)dlsym(RTLD_NEXT, "open64");
    int ret = 0;
    char long_path[100] = {0}, mode_str[10] = {0}, flag_str[10] = {0};
    char real_path[LEN] = {'\0'};
    get_real_path(pathname, real_path);
    sprintf(flag_str, "%o", flags);
        
    if(__OPEN_NEEDS_MODE (flags)){
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
        sprintf(mode_str, "%o", mode);
        ret = o_func(pathname, flags, mode);
    }
    else{
        ret = o_func(pathname, flags);
        sprintf(mode_str, "%o", mode);
    }
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\", %s, %s) = %d\n", LOGGER, __func__, string(real_path).c_str(), flag_str, mode_str, ret);
    print(string(result));
    return ret;
}

int remove(const char *pathname){
    o_remove o_func = (o_remove)dlsym(RTLD_NEXT, "remove");
    int ret = o_func(pathname);
    char real_path[100] = {'\0'};
    get_real_path(pathname, real_path);
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s(\"%s\") = %d\n", LOGGER, __func__, real_path, ret);
    print(string(result));
    return ret;
}

int rename(const char *oldpath, const char *newpath){
    o_rename o_func = (o_rename)dlsym(RTLD_NEXT, "rename");
    int ret = o_func(oldpath, newpath);
    
    char real_path1[100] = {'\0'};
    get_real_path(oldpath, real_path1);
    char real_path2[100] = {'\0'};
    get_real_path(newpath, real_path2);

    char result[LEN];
    sprintf(result, "%s %s(\"%s\", \"%s\") = %d\n", LOGGER, __func__, real_path1, real_path2, ret);
    print(string(result));
    return ret;
}

FILE* tmpfile(void){
    o_tmpfile o_func = (o_tmpfile)dlsym(RTLD_NEXT, "tmpfile");
    FILE* ret = o_func();
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s() = %p\n", LOGGER, __func__, ret);
    print(string(result));
    return ret;
}

FILE* tmpfile64(void){
    o_tmpfile64 o_func = (o_tmpfile64)dlsym(RTLD_NEXT, "tmpfile64");
    FILE* ret = o_func();
    char result[LEN] = {'\0'};
    sprintf(result, "%s %s() = %p\n", LOGGER, __func__, ret);
    print(string(result));
    return ret;
}
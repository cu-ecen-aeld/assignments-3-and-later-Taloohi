#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    openlog(NULL, LOG_PID, LOG_USER);

    syslog(LOG_DEBUG, "Number of input arguments is %d\n", argc-1);

    if (argc != 3){
        // LOG HERE
        syslog(LOG_ERR,"Error, size of input arguments is %d, expected 2 input arguments\n", argc-1);
        return 1;
    }

    const char *file_path = argv[1];
    const char *str_to_write = argv[2];

    syslog(LOG_DEBUG,"Writing %s to file %s\n", str_to_write, file_path);

    int fd;

    fd = creat(file_path, 0644);
    if (fd == -1){
        // LOG ERROR
        syslog(LOG_ERR,"Failed to open file in path %s\n", file_path);
    }
    else{
        syslog(LOG_DEBUG,"Successfully opened filepath %s\n", file_path);
    }
    ssize_t nr;
    nr = write(fd, str_to_write, strlen(str_to_write));
    if (nr == -1){
        syslog(LOG_ERR,"Failed to write string (%s) to path (%s)\n", str_to_write, file_path);
    }
    else{
        syslog(LOG_DEBUG,"Successfully wrote string (%s) to path (%s)\n", str_to_write, file_path);
    }
    // return 0;
}



#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "/tmp/buf.txt"
int sort_start(int fd[2]) {
    int r;
    int w;
    w = open(FILENAME,O_WRONLY|O_CREAT,0644);
    if (w<0) {
        perror("write");
        return -1;
    }
    r = open(FILENAME,O_RDONLY|O_CREAT,0644);
    if (w<0) {
        perror("read");
        return -1;
    }
    fd[0]=r;
    fd[1]=w;
    return 0;
}


int sort_end(int fd[2]) {
    pid_t p = fork();
    // printf("%d\n",p);
    if (p == 0) {
        char *arg[1];
        arg[1] = FILENAME;
        execlp("/bin/rm","rm",FILENAME,(char *)0);
    } else {
        int wstatus;
        if (wait(&wstatus) <0) {
            errno = 23;
            return -1;
        } else {
            return 0;
        }
    }
}

int read_lines(int fd,int do_print){
    char line[1024]; // Buffer to hold each line, adjust the size as needed
    while (fgets(line, sizeof(line), stdin) != NULL) {
        // Process or print the line
        if (do_print) {
             printf("%s", line);
        }  else {
            if (write(fd, line, strlen(line)) == -1) {
                perror("Error writing to file");
                close(fd);
                exit(-1);
            }
        }
    }
}


int sort_lines(int fd){
    pid_t p = fork();
    // printf("%d\n",p);
    if (p == 0) {
        dup2(fd,STDIN_FILENO);
        execlp("sort","sort",(char *)0);
    } else {
        int wstatus;
        if (wait(&wstatus) <0) {
            errno = 23;
            return -1;
        } else {
            return 0;
        }
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    int fd[2];
    if (sort_start(fd) < 0){
        printf("start sort failed");
        exit(-1);
    } 
    // cleanup stds
    int devNull = open("/dev/null",0);
    dup2(devNull,STDERR_FILENO);
    /// using write to directed unsorted text lines to fd 1
    read_lines(fd[1],0);
    close(fd[1]);
    /// sort data
    sort_lines(fd[0]);
    if (sort_end(fd) < 0){
        printf("end sort failed");
        exit(-1);
    } 
    return 0;
}

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
volatile sig_atomic_t running = 1;
volatile sig_atomic_t reload_requested = 0;

void sigterm_handler(int sig)
{
    running = 0;
}

void sighup_handler(int sig)
{
    reload_requested = 1;
}
int main()
{
        //first fork()
        int ret=fork();
        if(ret==-1)
        {
                printf("fork failed\n");
                return 0;
        }
        if(ret>0)
        {
                printf("parent\n");
                return 0;
        }
        if(ret==0)
        {
                printf("child\n");

        }
        //setsid
        if(setsid()<0)
        {
                exit(EXIT_FAILURE);
        }
//      printf("setsid success\n");
      //secondfork()
        ret=fork();
        if(ret==-1)
        {
                printf("fork failed\n");
                return 0;
        }
        if(ret>0)
        {
                printf("parent\n");
                return 0;
        }
        if(ret==0)
        {
                printf("child\n");

        }
        if(chdir("/")<0)
        {
                exit(EXIT_FAILURE);
        }
        umask(0);
        int fd=open("/dev/null",O_RDWR);
        dup2(fd,STDIN_FILENO);
        dup2(fd,STDOUT_FILENO);
        dup2(fd,STDERR_FILENO);
//      printf("V25HE10V2");
        if(fd>STDERR_FILENO)
                close(fd);
        pid_t pid=getpid();
        fd = open("/tmp/daemon.pid", O_CREAT | O_WRONLY,0664);
        struct flock lock;

        lock.l_type   = F_WRLCK;
        lock.l_whence = 0;
        lock.l_start  = 0;
        lock.l_len    = 0;

        if(fcntl(fd, F_SETLK, &lock) == -1)
       {
        printf("Daemon already running\n");
        close(fd);
        exit(1);
       }
        char buf[20];
        sprintf(buf, "%d\n", pid);
        write(fd,buf,strlen(buf));

        struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sa.sa_handler = sighup_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGHUP, &sa, NULL);
    int logfd = open("/tmp/daemon.log", O_CREAT | O_WRONLY | O_APPEND, 0664);

      while(running)
{
        char msg[50];
        if(reload_requested)
        {
          reload_requested=0;
          sprintf(msg,"SIGHUP received \n");
          write(logfd,msg,strlen(msg));
        }
        sprintf(msg, "Daemon running PID=%d\n", pid);
        write(logfd, msg, strlen(msg));
        sleep(5);
}
       close(logfd);
       close(fd);
       remove("/tmp/daemon.pid");
}

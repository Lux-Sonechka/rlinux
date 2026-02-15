#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/wait.h>

int main(void) {
    struct utsname sysinfo;
    uname(&sysinfo);

    // Logo ASCII simples
    //printf("\033[1;35m=======================\033[0m\n");
    //printf("\033[1;35m   RIOT LINUX FETCH    \033[0m\n");
    //printf("\033[1;35m=======================\033[0m\n\n");
    

    pid_t pid = fork();
    if (pid == 0) {
        execlp("bash", "bash", "-c",
               "/usr/local/bin/figlet 'RIOT LINUX' | /usr/local/bin/lolcat",
               (char *)NULL);
        perror("execlp");
        _exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork");
    }




    // Hostname
   char hostname[128];
   gethostname(hostname, sizeof(hostname));

    // Kernel
    printf("\033[1;32mHost:\033[0m    %s\n", hostname);
    //printf("########################################\n ");
    printf("\033[1;32mKernel:\033[0m  %s %s\n", sysinfo.sysname, sysinfo.release);
    printf("\033[1;32mArch:\033[0m    %s\n", sysinfo.machine);
    printf("\033[1;32mShell:\033[0m    %s\n", "bash");
    printf("\033[1;32mOS:\033[0m    %s\n", "Riot Linux Experimental");
    printf("\033[1;32mTerminal:\033[0m    %s\n", "/dev/tty1");
    printf("\033[1;32mResolution:\033[0m    %s\n", "preferred");



    
    // Uptime (lendo /proc/uptime)
    FILE *f = fopen("/proc/uptime", "r");
    if (f) {
        double up;
        if (fscanf(f, "%lf", &up) == 1) {
            int hours = (int)(up / 3600);
            int mins = (int)((up - hours*3600) / 60);
            printf("\033[1;32mUptime:\033[0m  %dh %dm\n", hours, mins);
        }
        fclose(f);
    }

    // Memória total (lendo /proc/meminfo)
    f = fopen("/proc/meminfo", "r");
    if (f) {
        char label[64];
        long mem;
        while (fscanf(f, "%63s %ld", label, &mem) == 2) {
            if (strcmp(label, "MemTotal:") == 0) {
                printf("\033[1;32mMemory:\033[0m  %ld MB\n", mem / 1024);
                break;
            }
        }
        fclose(f);
    }

    return 0;
}


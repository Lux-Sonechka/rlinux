#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/vt.h>


void clear_screen() {
    // \033[H -> move cursor para o topo
    // \033[2J -> limpa a tela
    write(1, "\033[H\033[2J", 7);
}
int check_shell(const char *path) {
    if (access(path, X_OK) == 0) {
	write(1, "Found bash!", 10); 
        return 1;
    } else {
        write(1, "Erro: /bin/bash nao  encontrado ou nao  executavel!", 50);
        return 0;
    }
}
void show_init() {
	char *argv[] = {"figlet", "RIOT LINUX", NULL};
	execvp("/usr/local/bin/figlet", argv);
	perror("execvp");
}


int main(void) {
	
    signal(SIGCHLD, SIG_IGN);
    
    // Mounts filesystems
    mkdir("/proc", 0555);
    mkdir("/sys", 0555);
    mkdir("/dev", 0555);

    mount("proc", "/proc", "proc", 0, "");
    mount("sysfs", "/sys", "sysfs", 0, "");
    mount("devtmpfs", "/dev", "devtmpfs", 0, "");

    // Runs TTY console or VGA
    setsid();
    
    int c = open("/dev/console", O_RDWR);

    dup2(c, 0);
    dup2(c, 1);
    dup2(c, 2);
    
    if (c > 2) close(c);

    // Now comes TTY !!

    int t = open("/dev/tty1", O_RDWR);
    ioctl(t, TIOCSCTTY, 0);

    dup2(t, 0);
    dup2(t, 1);
    dup2(t, 2);

    if (t > 2) close(t);


    // Cleans mess
    clear_screen();
    // bugs screen, don't do! -> show_init();

    // Welcum :3
    write(1, "Welcome to Riot Linux - Your no-hat Linux distro!\n", 50);
    
    // Runs bash shell
    // char *argv[] = { "/bin/bash", NULL };
    // execv(argv[0], argv);
    execl("/bin/bash", "bash", "-i", NULL);

    // If bash is not found, you'll get an error
    perror("exec bash failed");

    check_shell("/bin/bash");
    write(1, "Bash shell not found. Entering loop.", 35);

    for (;;);
}



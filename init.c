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
#include <sys/wait.h>
#include <string.h>
#include <crypt.h>

const char *red = "\033[31m"; // vermelho 
const char *reset = "\033[0m"; // reset 
const char *green = "\e[36m";


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
    pid_t pid = fork();
    if (pid == 0) {
        // processo filho
        execlp("bash", "bash", "-c",
               "/usr/local/bin/figlet 'RIOT LINUX' | /usr/local/bin/lolcat",
               (char *)NULL);
        perror("execlp");
        _exit(1);
    } else if (pid > 0) {
        // processo pai espera o filho terminar
        wait(NULL);
    } else {
        perror("fork");
    }
}

volatile sig_atomic_t interrupted = 0;

void sigint_handler(int sig) {
    interrupted = 1; // marca que houve Ctrl+C
}

void run_login() {
    int status;
    while (1) {
        interrupted = 0; // reseta flag
        pid_t pid = fork();
        if (pid == 0) {
            // processo filho: executa o binário login
            execl("/usr/bin/rlogin", "rlogin", (char *)NULL);
            perror("execl login failed");
            _exit(1);
        } else if (pid > 0) {
            // processo pai: espera o login terminar
            wait(&status);

            if (interrupted) {
                printf("\nLogin interrupted....\n");
                continue; // volta ao loop
            }

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                // login retornou sucesso (exit 0)
                break;
            } else {
                printf("Login failed, try again...\n");
            }
        } else {
            perror("fork");
            exit(1);
        }
    }
}

int main(void) {
	
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, sigint_handler);
    
    // Mounts filesystems
    mkdir("/proc", 0555);
    mkdir("/sys", 0555);
    mkdir("/dev", 0555);

    mount("proc", "/proc", "proc", 0, "");
    mount("sysfs", "/sys", "sysfs", 0, "");
    mount("devtmpfs", "/dev", "devtmpfs", 0, "");
    
    if (mount(NULL, "/", NULL, MS_REMOUNT, NULL) < 0) {
    	perror("### Can't mount  root as rw.");
    }
    if (sethostname("riot-linux", 10) != 0) {
	perror("sethostname");
    }


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
    // bugs screen, don't do! ->
    show_init();


    // Welcum :3
    write(1, "Welcome to Riot Linux - Your no-hat Linux distro!\n", 50);   
    // Runs bash shell
    // char *argv[] = { "/bin/bash", NULL };
    // execv(argv[0], argv);
    run_login();
    char username[64];
        FILE *f = fopen("/tmp/logged_user", "r");
	if (f) {
        	if (fgets(username, sizeof(username), f)) {
        		username[strcspn(username, "\n")] = 0; // remove \n
        		setenv("USER", username, 1);
        		char prompt[128];
        		snprintf(prompt, sizeof(prompt), "\\e[36m %s \e[0m[\\w]$ ", username);
        		setenv("PS1", prompt, 1);
    		}

    		fclose(f);
    		remove("/tmp/logged_user"); // apaga o arquivo
	}
    chdir("/home/");
    execl("/bin/bash", "bash", "-i", NULL);

    // If bash is not found, you'll get an error
    perror("exec bash failed");

    check_shell("/bin/bash");
    write(1, "Bash shell not found. Entering loop.", 35);

    for (;;);
}



#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

#define USERFILE "/usr/etc/users.db"
char username[64];
int main(void) {
    char user[64], pass[64], line[256];

    while (1) {
        printf("login: ");
        if (!fgets(user, sizeof(user), stdin)) exit(1);
        user[strcspn(user, "\n")] = 0;
	strncpy(username, user, sizeof(username));
	username[sizeof(username)-1] = '/0';

        printf("password: ");
        if (!fgets(pass, sizeof(pass), stdin)) exit(1);
        pass[strcspn(pass, "\n")] = 0;

        FILE *f = fopen(USERFILE, "r");
        if (!f) {
            perror("users.db");
            exit(1);
        }

        int ok = 0;
        while (fgets(line, sizeof(line), f)) {
            char *name = strtok(line, ":");
            char *hash = strtok(NULL, ":");
            char *isadm = strtok(NULL, ":\n");

            if (name && hash && strcmp(name, user) == 0) {
                // Caso especial: root sem senha
                if (strcmp(name, "root") == 0 && strlen(hash) == 0) {
                    ok = 1;
                    break;
                }
                // Valida senha com crypt()
                char *test = crypt(pass, hash);
                if (test && strcmp(test, hash) == 0) {
                    ok = 1;
                    break;
                }
            }
        }

        fclose(f);

        if (ok) {

	    FILE *f = fopen("/tmp/logged_user", "w");
	    if (f) {
    		fprintf(f, "%s\n", username);
    		fclose(f);
	    }

            printf("Login successful!\n");
            return 1; // sucesso → sai do programa
        } else {
            printf("Login failed.\n");
        }
    }
}

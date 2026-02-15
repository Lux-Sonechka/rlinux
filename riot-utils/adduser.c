#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>


#define USERFILE "/usr/etc/users.db"

int main(int argc, char *argv[]) {
    char *name = NULL;
    char *password = NULL;
    char *isadm = "no";

    // Parse dos parâmetros
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--name") == 0 && i + 1 < argc) {
            name = argv[++i];
        } else if (strcmp(argv[i], "--password") == 0 && i + 1 < argc) {
            password = argv[++i];
        } else if (strcmp(argv[i], "--isadm") == 0 && i + 1 < argc) {
            isadm = argv[++i];
        }
    }

    if (!name || !password) {
        fprintf(stderr, "Help: adduser --name <name> --password <password> [--isadm yes|no]\n");
        return 1;
    }

    // Gera hash da senha usando SHA-512 via crypt()
    // Salt começa com "$6$" para indicar SHA-512
    char salt[32];
    snprintf(salt, sizeof(salt), "$6$%s", "mysalt"); // pode gerar salt aleatório depois

    char *hash = crypt(password, salt);
    if (!hash) {
        perror("Error generating hash.");
        return 1;
    }

    // Salva no arquivo
    FILE *f = fopen(USERFILE, "a");
    if (!f) {
        perror("Error opening user database file.");
        return 1;
    }
    fprintf(f, "%s:%s:%s\n", name, hash, isadm);
    fclose(f);

    printf("User '%s' added succesfully..\n", name);
    return 0;
}

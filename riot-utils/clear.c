#include <stdio.h>

int main(void) {
    // ANSI escape: limpa tela e move cursor para o topo
    printf("\033[2J\033[H");
    return 0;

}

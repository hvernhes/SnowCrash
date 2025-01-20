#include <stdio.h>

int main(int ac, char **av) {

        int i = 0;
        char c;
        while (av[1][i] != 0) {
                c = av[1][i];
                printf("%c", (c - i));
                i++;
        }
        printf("\n");
        return 0;
}
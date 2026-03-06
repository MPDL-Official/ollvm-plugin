#include <stdio.h>
#include <string.h>

const char *secret = "hello_ollvm";

int check(const char *input) {
    if (strcmp(input, secret) == 0) {
        return 1;
    }
    for (int i = 0; i < 10; i++) {
        if (input[i] != secret[i]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    printf("result: %d\n", check("test"));
    return 0;
}

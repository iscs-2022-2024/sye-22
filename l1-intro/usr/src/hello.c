

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *s = "Hello world !";
    size_t len = strlen(s);

	printf("SO3 - Simple application\n");
    printf("%s\n", s);
    printf("Address: %p\n", s);

    // modifying byte by byte
    // address: 0x8000

    for (size_t i = 0; i < len; ++i)
        ++*(char*)(0x8000 + i);

    // after modification
    printf("%s\n", s);

    return 0;
}

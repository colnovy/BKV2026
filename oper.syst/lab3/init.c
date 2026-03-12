#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_NAME "seats.dat"
#define SIZE 28

int main() {
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error creating file");
        return 1;
    }
    unsigned char zero = 0;
    for (int i = 0; i < SIZE; i++) {
        if (write(fd, &zero, 1) != 1) {
            perror("writing error");
            close(fd);
            return 1;
        }
    }
    close(fd);
    printf("File %s was created with %d null bytes (all space is free).\n", FILE_NAME, SIZE);
    return 0;
}
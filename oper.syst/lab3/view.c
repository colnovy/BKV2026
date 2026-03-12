#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define FILE_NAME "seats.dat"
#define ROWS 7
#define COLS 4

int main() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    printf("Seat plan (rows 1..%d, seats 1..%d):\n", ROWS, COLS);
    printf("-------------------------------------------------\n");

    for (int r = 1; r <= ROWS; r++) {
        printf("Row %d: ", r);
        for (int p = 1; p <= COLS; p++) {
            off_t offset = (r - 1) * COLS + (p - 1);
            unsigned char status;
            if (pread(fd, &status, 1, offset) != 1) {
                perror("reading error");
                close(fd);
                return 1;
            }

            struct flock test_lock = {
                .l_type = F_WRLCK,      
                .l_whence = SEEK_SET,
                .l_start = offset,
                .l_len = 1,
                .l_pid = 0
            };
            int locked = 0;
            if (fcntl(fd, F_GETLK, &test_lock) == 0) {

                if (test_lock.l_type != F_UNLCK) {
                    locked = test_lock.l_pid;  
                }
            }

            char state_char = (status == 0) ? '0' : 'X'; 
            if (locked) {
                printf("[%c (unit PID=%d)] ", state_char, locked);
            } else {
                printf("[%c] ", state_char);
            }
        }
        printf("\n");
    }

    printf("0 – свободно, X – occupied\n");
    close(fd);
    return 0;
}
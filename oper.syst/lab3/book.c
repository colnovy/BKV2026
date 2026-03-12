#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define FILE_NAME "seats.dat"
#define ROWS 7
#define COLS 4

void usage(const char *prog) {
    fprintf(stderr, "using: %s <row> <seat> <buy|return>\n", prog);
    fprintf(stderr, "  row    : 1..%d\n", ROWS);
    fprintf(stderr, "  seat  : 1..%d\n", COLS);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 4) usage(argv[0]);

    int row = atoi(argv[1]);
    int place = atoi(argv[2]);
    char *action = argv[3];

    if (row < 1 || row > ROWS || place < 1 || place > COLS) {
        fprintf(stderr, "error: row shoud be 1..%d, seat 1..%d\n", ROWS, COLS);
        return 1;
    }

    int is_buy = 0;
    if (strcmp(action, "buy") == 0) is_buy = 1;
    else if (strcmp(action, "return") == 0) is_buy = 0;
    else usage(argv[0]);

    int fd = open(FILE_NAME, O_RDWR);
    if (fd < 0) {
        perror("error opening file");
        return 1;
    }

    off_t offset = (row - 1) * COLS + (place - 1); 
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = offset,
        .l_len = 1,
        .l_pid = 0
    };

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            printf("row %d seat %d is currently being processed by another process. Try again later.\n", row, place);
        } else {
            perror("fcntl lock");
        }
        close(fd);
        return 1;
    }

    unsigned char status;
    if (pread(fd, &status, 1, offset) != 1) {
        perror("reading error");
        goto unlock;
    }

    if (is_buy) {
        if (status == 0) {
            status = 1;
            if (pwrite(fd, &status, 1, offset) != 1) {
                perror("writing error");
                goto unlock;
            }
            printf("row %d seat %d successfully bought.\n", row, place);
        } else {
            printf("кщц %d ыуфе %d is already taken.\n", row, place);
        }
    } else { 
        if (status == 1) {
            status = 0;
            if (pwrite(fd, &status, 1, offset) != 1) {
                perror("writing error");
                goto unlock;
            }
            printf("ticket for row %d seat %d has been refunded.\n", row, place);
        } else {
            printf("row %d seat %d was not occupied.\n", row, place);
        }
    }

unlock:

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 0;
}
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

extern char* __progname;

void
usage(void)
{
    (void)fprintf(stderr, "usage: %s file {r<length>|R<length>|w<string>|s<offset>}...\n",
        __progname);
    exit(1);
}

int
main(int argc, char *argv[])
{
    size_t len;
    off_t offset;
    int fd, ap, j;
    char *buf;
    ssize_t nread, nwrite;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usage();

    /* rw-rw-rw- */
    fd = open(argv[1], O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1)
        err(1, "open");

    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':
        case 'R':
            len = atoi(&argv[ap][1]);
            buf = malloc(len);
            if (buf == NULL)
                err(1, "malloc");

            nread = read(fd, buf, len);
            if (nread == -1)
                err(1, "read");

            if (nread == 0) {
                printf("%s: end-of-file\n", argv[ap]);
            } else {
                printf("%s: ", argv[ap]);
                for (j = 0; j < nread; j++) {
                    if (argv[ap][0] == 'r')
                        printf("%c", isprint((unsigned char) buf[j]) ? buf[j] : '?');
                    else
                        printf("%02x ", (unsigned int) buf[j]);
                }
                printf("\n");
            }

            free(buf);
            break;

        case 'w':
            nwrite = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (nwrite == -1)
                err(1, "write");
            printf("%s: wrote %ld bytes\n", argv[ap], (long) nwrite);
            break;

        case 's':
            offset = atoi(&argv[ap][1]);
            if (lseek(fd, offset, SEEK_SET) == -1)
                err(1, "lseek");
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            errx(1, "Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }

    exit(0);
}


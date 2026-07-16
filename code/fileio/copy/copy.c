// open() in fcntl.h
// read(), write(), close() in unistd.h
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

void
usage(char *prog)
{
    printf("%s old-file new-file\n", prog);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int ifd, ofd, flags;
    mode_t perms;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usage(argv[0]);

    /* Open input and output files */

    ifd = open(argv[1], O_RDONLY);
    if (ifd == -1)
    {
        printf("opening file fail: %s(%d)\n", argv[1], errno);
        exit(EXIT_FAILURE);
    }
   
    flags = O_CREAT | O_WRONLY | O_TRUNC;
    perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
    ofd = open(argv[2], flags, perms);
    if (ofd == -1)
    {
        printf("opening file fail: %s(%d)\n", argv[1], errno);
        exit(EXIT_FAILURE);
    }

    /* Transfer data until we encounter end of input or an error */

    while ((nread = read(ifd, buf, BUF_SIZE)) > 0)
    {
        if (write(ofd, buf, nread) != nread)
        {
            printf("couldn't write whole buffer\n");
            exit(EXIT_FAILURE);
        }
    }

    if (nread == -1)
    {
        printf("read error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    close(ifd);
    close(ofd);

    exit(EXIT_SUCCESS);
}


// open() in fcntl.h
// read(), write(), close() in unistd.h
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

extern char* __progname;

void
usage(void)
{
    (void)fprintf(stderr, "usage: %s old-file new-file\n", __progname);
    exit(1);
}

int
main(int argc, char *argv[])
{
    int ifd, ofd, flags;
    mode_t perms;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usage();

    /* Open input and output files */

    ifd = open(argv[1], O_RDONLY);
    if (ifd == -1)
        err(1, "open file fail - %s", argv[1]);
   
    flags = O_CREAT | O_WRONLY | O_TRUNC;
    perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
    ofd = open(argv[2], flags, perms);
    if (ofd == -1)
        err(1, "open file fail - %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    while ((nread = read(ifd, buf, BUF_SIZE)) > 0)
        if (write(ofd, buf, nread) != nread)
            err(1, "couldn't write whole buffer");

    if (nread == -1)
        err(1, "read");

    if (close(ifd) == -1)
        err(1, "close input");
    if (close(ofd) == -1)
        err(1, "close output");

    exit(0);
}


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <imsg.h>   /* need -lutil */

enum imsg_type {
    IMSG_DATA = 1,
};

struct my_data {
    int value;
    char name[32];
};

int child_main(struct imsgbuf *ibuf)
{
    struct imsg imsg;
    struct my_data data;
    ssize_t n;

    data.value = 42;
    snprintf(data.name, sizeof(data.name), "Hello from child!");

    if (imsg_compose(ibuf, IMSG_DATA, 0, 0, -1, &data, sizeof(data)) == -1)
        err(1, "child: imsg_compose");

    /* send msg to parent */
    if (imsgbuf_flush(ibuf) == -1)
        err(1, "child: imsgbuf_flush");

    printf("child: msg sent.\n");

    for (;;) {
        if (imsgbuf_read(ibuf) == -1)
            err(1, "child: imsgbuf_read");

        if (imsgbuf_get(ibuf, &imsg) == -1)
            err(1, "child: imsgbuf_get");

        if (imsg.hdr.type == IMSG_DATA) {
            if (imsg.hdr.len - IMSG_HEADER_SIZE != sizeof(data)) {
                printf("child: recv malformed data.\n");
                imsg_free(&imsg);
                continue;
            }
            memcpy(&data, imsg.data, sizeof(data));
            printf("child: recv data, value=%d, name='%s'\n", data.value, data.name);
            imsg_free(&imsg);
            break;
        }
        imsg_free(&imsg);
    }

    return 0;
}

int parent_main(struct imsgbuf *ibuf)
{
    struct imsg imsg;
    struct my_data data;
    ssize_t n;

    for (;;) {
        if (imsgbuf_read(ibuf) == -1)
            err(1, "parent: imsgbuf_read");

        if (imsgbuf_get(ibuf, &imsg) == -1)
            err(1, "parent: imsgbuf_get");

        if (imsg.hdr.type == IMSG_DATA) {
            if (imsg.hdr.len - IMSG_HEADER_SIZE != sizeof(data)) {
                printf("parent: recv malformed data.\n");
                imsg_free(&imsg);
                continue;
            }
            memcpy(&data, imsg.data, sizeof(data));
            printf("parent: recv data, value=%d, name='%s'\n", data.value, data.name);
            imsg_free(&imsg);
            break;
        }
        imsg_free(&imsg);
    }

    /* send a msg to client */
    data.value = 100;
    snprintf(data.name, sizeof(data.name), "Reply from parent!");

    if (imsg_compose(ibuf, IMSG_DATA, 0, 0, -1, &data, sizeof(data)) == -1)
        err(1, "parent: imsg_compose");

    if (imsgbuf_flush(ibuf) == -1)
        err(1, "parent: imsg_flush");

    printf("parent: reply sent.\n");
    return 0;
}

int main(int argc, char* argv[])
{
    struct imsgbuf parent_ibuf, child_ibuf;
    int imsg_fds[2];
    pid_t pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, imsg_fds) == -1)
        err(1, "socketpair");

    switch (pid = fork()) {
    case -1:
        err(1, "fork");
    case 0:
        /* child */
        close(imsg_fds[0]);
        if (imsgbuf_init(&child_ibuf, imsg_fds[1]) == -1)
            err(1, "child: imsgbuf_init");
        exit(child_main(&child_ibuf));
    default:
        /* parent */
        close(imsg_fds[1]);
        if (imsgbuf_init(&parent_ibuf, imsg_fds[0]) == -1)
            err(1, "parent: imsgbuf_init");
        parent_main(&parent_ibuf);
        wait(NULL); /* wait until child exits */
        break;
    }

    return 0;
}


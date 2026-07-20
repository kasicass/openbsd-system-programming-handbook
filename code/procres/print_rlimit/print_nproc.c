#include <err.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern char* __progname;

void
usage(void)
{
    fprintf(stderr, "%s soft-limit [hard-limit]\n", __progname);
    exit(1);
}

int
print_rlimit(const char *msg, int resource)
{
    struct rlimit rlim;

    if (getrlimit(resource, &rlim) == -1)
        return -1;

    printf("%s soft=", msg);
    if (rlim.rlim_cur == RLIM_INFINITY)
        printf("infinite");
    else
        printf("%lld", (long long) rlim.rlim_cur);

    printf("; hard=");
    if (rlim.rlim_max == RLIM_INFINITY)
        printf("infinite\n");
    else
        printf("%lld\n", (long long) rlim.rlim_max);

    return 0;
}

int
main(int argc, char *argv[])
{
    struct rlimit rl;
    int j;
    pid_t child_pid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0)
        usage();

    print_rlimit("Initial maximum process limits: ", RLIMIT_NPROC);

    /* Set new process limits (hard == soft if not specified) */

    rl.rlim_cur = (argv[1][0] == 'i') ? RLIM_INFINITY : atoi(argv[1]);
    rl.rlim_max = (argc == 2) ? rl.rlim_cur :
        ((argv[2][0] == 'i') ? RLIM_INFINITY : atoi(argv[2]));
    if (setrlimit(RLIMIT_NPROC, &rl) == -1)
        err(1, "setrlimit");

    print_rlimit("New maximum process limits:     ", RLIMIT_NPROC);

    /* Create as many children as possible */

    for (j = 1; ; j++) {
        switch (child_pid = fork()) {
        case -1:
            err(1, "fork");

        case 0:
            exit(0); /* Child */

        default:
            /* Parent: display message about each new child and let
               the resulting zombies accumulate */
            printf("Child %d (PID=%ld) started\n", j, (long) child_pid);
        }
    }

    return 0;
}


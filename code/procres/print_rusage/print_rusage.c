#include <err.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>

#define PRINT_LONG(field) printf(#field ": %ld\n", (ru->ru_##field));

void
print_ru(struct rusage *ru)
{
    printf("utime:  %lld.%06llds\n", (long long)ru->ru_utime.tv_sec, (long long)ru->ru_utime.tv_usec);
    printf("stime:  %lld.%06llds\n", (long long)ru->ru_stime.tv_sec, (long long)ru->ru_stime.tv_usec);
    PRINT_LONG(maxrss);
    PRINT_LONG(ixrss);
    PRINT_LONG(idrss);
    PRINT_LONG(isrss);
    PRINT_LONG(minflt);
    PRINT_LONG(majflt);
    PRINT_LONG(nswap);
    PRINT_LONG(inblock);
    PRINT_LONG(oublock);
    PRINT_LONG(msgsnd);
    PRINT_LONG(msgrcv);
    PRINT_LONG(nsignals);
    PRINT_LONG(nvcsw);
    PRINT_LONG(nivcsw);
}

int
main()
{
    struct rusage ru;

    if (getrusage(RUSAGE_SELF, &ru) == -1)
        err(1, "getrusage");

    printf("==== RUSAGE_SELF ====\n");
    print_ru(&ru);
    printf("\n");

    if (getrusage(RUSAGE_THREAD, &ru) == -1)
        err(1, "getrusage");

    printf("==== RUSAGE_THREAD ====\n");
    print_ru(&ru);

    return 0;
}


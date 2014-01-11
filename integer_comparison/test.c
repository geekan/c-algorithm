#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))
#define TESTNUM 2000

int f1(int a, int b)
{
    return (a > b) | -(b > a);
}

int f2(int a, int b)
{
    return (a > b) - (a < b);
}

int f3(int a, int b)
{
    __asm__ __volatile__ (
        "sub %1, %0 \n\t"
        "jno 1f \n\t"
        "cmc \n\t"
        "rcr %0 \n\t"
        "1: "
    : "+r"(a)
    : "r"(b)
    : "cc");
    return a;
}

int compare1(int a, int b)
{
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
}
int compare2(int a, int b)
{
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

typedef int (*compare_func)(int a, int b);
struct finfo {
    compare_func func;
    char name[16];
};

struct finfo farr[] = {
    {f1, "f1"},
    {f2, "f2"},
    {f3, "f3"},
    {compare1, "compare1"},
    {compare2, "compare2"},
};

int main()
{
    int i, a, b, r;
    int ret;
    int fsize = ARRAY_SIZE(farr);
    struct timespec start = {0};
    struct timespec end = {0};
    srand(time(NULL));

    int random_int[TESTNUM];
    for (i = 0; i < TESTNUM; i++) {
        r = rand();
        random_int[i] = r;
    }

    for (i = 0; i < fsize; i++) {
        compare_func compare = farr[i].func;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
        for (a = 0; a < TESTNUM; a++)
            for (b = 0; b < TESTNUM; b++)
                ret += compare(a, b);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        printf("%s: diff:%ld, start:%ld, end:%ld, ret:%d\n",
                farr[i].name, end.tv_nsec - start.tv_nsec, start.tv_nsec, end.tv_nsec, ret);
    }
    return 0;
}

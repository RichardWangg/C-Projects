#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int count = 0;
pthread_mutex_t count_mutex;

typedef struct
{
    int n;
    int init_pos;
} QueenThreadArgs;

int safe(char *config, int i, int j)
{
    int r, s;
    for (r = 0; r < i; r++)
    {
        s = config[r];
        if (j == s || i - r == j - s || i - r == s - j)
            return 0;
    }
    return 1;
}

void nqueens(char *config, int n, int i)
{
    char *new_config;
    int j;

    if (i == n)
    {
        pthread_mutex_lock(&count_mutex);
        count++;
        pthread_mutex_unlock(&count_mutex);
        return;
    }

    for (j = 0; j < n; j++)
    {
        new_config = malloc((i + 1) * sizeof(char));
        memcpy(new_config, config, i * sizeof(char));
        if (safe(new_config, i, j))
        {
            new_config[i] = j;
            nqueens(new_config, n, i + 1);
        }
        free(new_config);
    }
}

void *thread_func(void *arg)
{
    QueenThreadArgs *targ = (QueenThreadArgs *)arg;
    int n = targ->n;
    int init_pos = targ->init_pos;

    char *config = malloc(n * sizeof(char));
    config[0] = init_pos;

    nqueens(config, n, 1);
    free(config);

    return NULL;
}

int main(int argc, char *argv[])
{
    int n, i;
    pthread_t *threads;
    QueenThreadArgs *thread_args;

    if (argc < 2)
    {
        printf("%s: Enter num of queens\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);

    threads = malloc(n * sizeof(pthread_t));
    thread_args = malloc(n * sizeof(QueenThreadArgs));

    pthread_mutex_init(&count_mutex, NULL);

    for (i = 0; i < n; i++)
    {
        thread_args[i].n = n;
        thread_args[i].init_pos = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_args[i]);
    }

    for (i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&count_mutex);
    free(threads);
    free(thread_args);

    printf("Num Solutions: %d\n", count);

    return 0;
}

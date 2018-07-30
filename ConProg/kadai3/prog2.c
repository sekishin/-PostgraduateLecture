#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#define genrnd(mn, mx) (rand() % ((mx)-(mn)+1) + (mn))
#define THNUM 100    // 最大スレッド数


// ring buffer
struct ringbuf{
    int bufsize; // The size of ring buffer. If minus values indicates illegal state.
    int wptr, rptr;
    int n_item;  // The number of items in ring buffer
    int buf[1];  // For variable array size
} *rbuf;


typedef struct {
    int no;
    int size;
} Params;


pthread_mutex_t lockval;

void producer(Params *p)
{
    int rnd;
    int prod_No = p->no;
    int pnum = p-> size;
    #ifdef TIME
    printf("I am producer thread %d.\n", prod_No);
    #endif
    srand(time(NULL) ^ (prod_No << 8));
    for (; pnum; pnum--) {
        rnd = genrnd(20,80);
        // put random number into ring buffer
        while (1) {
            pthread_mutex_lock(&lockval);
            if (rbuf->bufsize > rbuf->n_item) break;
            if (rbuf->bufsize < 0) {  // illegal state occurs and operation stops
                pthread_mutex_unlock(&lockval);
                return;
            }
            usleep(1); // reduce waste of CPU resource
            pthread_mutex_unlock(&lockval);
        }
        rbuf->buf[rbuf->wptr++] = rnd;
        rbuf->wptr %= rbuf->bufsize;
        rbuf->n_item++;
        #ifdef TIME
        printf("P#%02d puts %2d, #item is %3d\n", prod_No, rnd, rbuf->n_item);
        #endif
        fflush(stdout);
        pthread_mutex_unlock(&lockval);
        rnd = genrnd(20,80);
        usleep(rnd*1000);
    }
    pthread_exit(0);
}

void consumer(Params *p)
{
    int rnd;
    int cons_No = p->no;
    int cnum = p->size;
    
    #ifdef TIME
    printf("I am consuer thread %d.\n", cons_No);
    #endif
    for (; cnum; cnum--) {
        // pick number from ring buffer
        while (1) {
            pthread_mutex_lock(&lockval);
            if (rbuf->n_item) break;
            if (rbuf->bufsize < 0) {  // illegal state and stop operation
                pthread_mutex_unlock(&lockval);
                return;
            }
            usleep(1); // reduce waste of CPU
            pthread_mutex_unlock(&lockval);
        }
        rnd = rbuf->buf[rbuf->rptr++];
        rbuf->rptr %= rbuf->bufsize;
        rbuf->n_item--;
        #ifdef TIME
        printf("C#%02d gets %d, #item is %3d\n", cons_No, rnd, rbuf->n_item);
        #endif
        fflush(stdout);
        pthread_mutex_unlock(&lockval);
        usleep(rnd*1000);
    }
    pthread_exit(0);
}


void err_msg(char *msg)
{
    puts(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int N;    // size of ring buffer
    int l;    // number of data produced by a producer process
    int L;    // number of producer processes
    int m;    // number of data consumed by a consumer process
    int M;    // number of cunsumer processes
    int n_prod, n_cons;
    int status;
    int pnum;
    int i;
    pthread_t p_threads[THNUM];
    pthread_t c_threads[THNUM];
    Params p_params[THNUM];
    Params c_params[THNUM];
    
    if (argc != 6) {
        fprintf(stderr, "Usage: %s N l L m M\n", argv[0]);
        err_msg("N: size of ring buffer\nl; number of data produced by a producer process\n L; number of producer processes\nm; number of data consumed by a consumer process\nM; number of cunsumer processes");
    }
    N = atoi(argv[1]);
    l = atoi(argv[2]);
    L = atoi(argv[3]);
    m = atoi(argv[4]);
    M = atoi(argv[5]);
    if (N <= 0 || l*L != m*M) {
        err_msg("Parameter error");
    }
    #ifdef TIME
    printf("N:%d, l:%d, L:%d, m:%d, M:%d\n", N, l, L, m, M);
    #endif
    
    // リングバッファと制御変数の確保
    rbuf = (struct ringbuf *)malloc(sizeof(struct ringbuf)); 
    if (rbuf == NULL) {
        err_msg("malloc");
    } 
    rbuf->bufsize = N;
    rbuf->n_item = 0;
    rbuf->wptr = rbuf->rptr = 0;
    pthread_mutex_init(&lockval, NULL);
    
    printf("N:%d, l:%d, L:%d, m:%d, M:%d\n", N, l, L, m, M);
    // create consumer
    n_cons = 0;
    while (n_cons < M) {
        Params tmp = {n_cons, m};
        c_params[n_cons] = tmp;
        pthread_create(&c_threads[n_cons], NULL, (void *)consumer, &c_params[n_cons]);
        #ifdef TIME
        printf("Thread consumer %d created. \n", n_cons);
        #endif
        n_cons++;
    }
    
    // create producer
    n_prod = 0;
    while(n_prod < L) {
        Params tmp = {n_prod, l};
        p_params[n_prod] = tmp;
        pthread_create(&p_threads[n_prod], NULL, (void *)producer, &p_params[n_prod]);
        #ifdef TIME
        printf("Thread producer %d created.\n", n_prod);
        #endif
        n_prod++;
    }
    // join threads
    for (pnum = 0; pnum < n_prod; pnum++) {
        pthread_join(p_threads[pnum], (void **)&status);
        #ifdef TIME
        printf("finish producer thread %d with return value %d\n", pnum, status);
        #endif
    }
    for (pnum = 0; pnum < n_cons; pnum++) {
        pthread_join(c_threads[pnum], (void **)&status);
        #ifdef TIME
        printf("finish consumer thread %d with return value %d\n", pnum, status);
        #endif
    }
    free(rbuf);
    return 0;
}

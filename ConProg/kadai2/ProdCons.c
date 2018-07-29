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

#define MAXPROC 100
#define genrnd(mn, mx) (rand() % ((mx)-(mn)+1) + (mn))
#define LOOP 100

double gettimeofday_msec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// ring buffer
struct ringbuf {
  int bufsize; // The size of ring buffer. If minus values indicates illegal state.
  int wptr, rptr;
  int n_item;  // The number of items in ring buffer
  int buf[1];  // For variable array size
} *rbuf;

// semaphor and shared memory
int semid;
int shmid;

void P()
{
  struct sembuf sops;
  
  sops.sem_num = 0;		// semaphore number
  sops.sem_op = -1;		// operation (decrement semaphore)
  sops.sem_flg = SEM_UNDO;	// The operarion is canceled when the calling porcess terminates
  semop(semid, &sops, 1);	// Omitt error handling
}

void V()
{
  struct sembuf sops;
  
  sops.sem_num = 0;		// semaphore number
  sops.sem_op = 1;		// operation (increment semaphore)
  sops.sem_flg = SEM_UNDO;	// The operarion is canceled when the calling porcess terminates
  semop(semid, &sops, 1);	// Omitt error handling
}

void producer(int prod_No, int pnum)
{
  int rnd;
#ifdef TIME
  printf("I am producer process.\n");
#endif
  srand(time(NULL) ^ (prod_No << 8));
  for (; pnum; pnum--) {
    rnd = genrnd(20,80);
    // put random number into ring buffer
    while (1) {
      P();
      if (rbuf->bufsize > rbuf->n_item) break;
      if (rbuf->bufsize < 0) {  // illegal state occurs and operation stops
        V();
        return;
      }
      usleep(1); // reduce waste of CPU resource
      V();
    }
    rbuf->buf[rbuf->wptr++] = rnd;
    rbuf->wptr %= rbuf->bufsize;
    rbuf->n_item++;
#ifdef TIME
    printf("P#%02d puts %2d, #item is %3d\n", prod_No, rnd, rbuf->n_item);
#endif
    fflush(stdout);
    V();
    rnd = genrnd(20,80);
    usleep(rnd*1000);
  }
}

void consumer(int cons_No, int cnum)
{
  int rnd;

#ifdef TIME
  printf("I am consuer process.\n");
#endif
  for (; cnum; cnum--) {
    // pick number from ring buffer
    while (1) {
      P();
      if (rbuf->n_item) break;
      if (rbuf->bufsize < 0) {  // illegal state and stop operation
        V();
        return;
      }
      usleep(1); // reduce waste of CPU
      V();
    }
    rnd = rbuf->buf[rbuf->rptr++];
    rbuf->rptr %= rbuf->bufsize;
    rbuf->n_item--;
#ifdef TIME
    printf("C#%02d gets %d, #item is %3d\n", cons_No, rnd, rbuf->n_item);
#endif
    fflush(stdout);
    V();
    usleep(rnd*1000);
  }
}

// release share memory and semaphore
void release()
{
  if (shmctl(shmid, IPC_RMID, NULL)) {
    perror("shmctl");
  }
  if (semctl(semid, 0, IPC_RMID)) {
    perror("semctl");
  }
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
  int pid;
  int status;
  int pnum, nid;
  int pmap[MAXPROC], cmap[MAXPROC];
  clock_t start, end;
  double t1, t2;
  time_t x1, x2;
  int i;

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
  shmid = shmget(IPC_PRIVATE, sizeof(struct ringbuf) + (N-1)*sizeof(int), IPC_CREAT | 0666);
  if (shmid == -1) {
    err_msg("shmget");
  }
  rbuf = (struct ringbuf *)shmat(shmid, NULL, 0);
  if (rbuf == (struct ringbuf *)-1) {
    err_msg("shmat");
  }
  rbuf->bufsize = N;
  rbuf->n_item = 0;
  rbuf->wptr = rbuf->rptr = 0;
  semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
  if (semid == -1) {
    perror("semget");
    if (shmctl(shmid, IPC_RMID, NULL)) { // release shared memory area
      perror("shmctl");
    }
    exit(1);
  }
  if (semctl(semid, 0, SETVAL, 1) == -1) {
    perror("semctl at initializing value of semaphore"); 
    release();
    exit(1);
  }

  start = clock();
  time(&x1);
  t1 = gettimeofday_msec();
  for (i=0; i<LOOP; i++) {
  //printf("N:%d, l:%d, L:%d, m:%d, M:%d\n", N, l, L, m, M);
  //printf("%d\n",i);
  // create consumer
  n_cons = 0;
  while (n_cons < M) {
    pid = fork();
    switch (pid) {
    case    0:
      consumer(n_cons, m);
      exit(0);
      break;
    case    -1:
      printf("Fork error\n");
      release();
      exit(1);
    default:
#ifdef TIME
       printf("Process id of consumer process %d is %d\n", n_cons, pid);
#endif
       cmap[n_cons] = pid;
       n_cons++;
     }
  }

  // create producer
  n_prod = 0;
  while(n_prod < L) {
    pid = fork();
    switch (pid) {
    case    0:
      producer(n_prod, l);
      exit(0);
      break;
    case    -1:
      printf("Fork error\n");
      rbuf->bufsize = -1;  // stop consumer
      for (pnum = 0; pnum < n_cons; pnum++) {
        pid = wait(&status); // wait for termination of consumer process
      }
      release();
      exit(1);
    default:
#ifdef TIME
      printf("Process id of producer process %d is %d\n", n_prod, pid);
#endif
      pmap[n_prod] = pid;
      n_prod++;
    }
  }
  // main process only reaches this position
  for (pnum = n_cons+n_prod; pnum > 0; pnum--) {
    pid = wait(&status);
    for (nid = 0; nid < n_cons; nid++) {
      if (pid == cmap[nid]) {
#ifdef TIME
        printf("Consumer %d finished at %ld\n", nid, time(NULL));
#endif
        break;
      }
    }
    if (nid != n_cons) continue;
    for (nid = 0; nid < n_prod; nid++) {
      if (pid == pmap[nid]) {
#ifdef TIME
        printf("Producer %d finished at %ld\n", nid, time(NULL));
#endif
        break;
      }
    }
    if (nid != n_prod) continue;
    printf("Illegal process ID %d\n", pid);
  }
  }
  t2 = gettimeofday_msec();
  time(&x2);
  end = clock();
  printf("processing time(clock): %.2fms\n", (double)(end-start)/LOOP);
  printf("processing time(gettimeofday): %.2fms\n", (t2-t1)/LOOP);
  printf("processing time(time): %.2fms\n", (x2-x1)/LOOP);
  release();
  return 0;
}

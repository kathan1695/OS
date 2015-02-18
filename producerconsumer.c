




#include <stdio.h>
#include <sys/wait.h>
#include "memory.h"
#include "semaphore.h"

#define N     10    /* size of the buffer                           */

#define MUTEX 2     /* use the first semaphore in the set for Mutex */
#define EMPTY 4     /* next semaphore will be empty                 */
#define FULL  6     /* and the one after that is for full           */

#define HEAD  N     /* put the head counter at position N           */
#define TAIL  N+1   /* and the tail at position N+1                 */

SEMAPHORE sem;
MEMORY memblk;

void producer(int);
void consumer(void);
void add(int *, int);
int del(int *);

main()
{
  int *buffer, i;

  setbuf(stdout,NULL);

  memblk=shm_create(sizeof(int)*(N+2));
  sem=sem_create();

  /* initialize the values of the semaphores */

  sem_set(sem, MUTEX, 1);
  sem_set(sem, EMPTY, N);
  sem_set(sem, FULL, 0);

  /* initialize the head and tail pointers, messy again!! */

  buffer=(int *)shm_attach(memblk);
  buffer[HEAD]=buffer[TAIL]=0;
  shm_detach((char *)buffer);

  if ((i=fork())==-1) {
    printf("unable to fork\n");
    sem_delete(sem);
    shm_destroy(memblk);
    exit(1);
  }

  if (i!=0) {
    producer(100);
    wait3(NULL,0,NULL);
    shm_destroy(memblk);
    sem_delete(sem);
    printf("main has terminated\n");
  }
  else
    consumer();
}

void producer(int n)
{
  int i, *buffer;

  buffer=(int *)shm_attach(memblk);

  for (i=0; i<n; i++) {
    sem_wait(sem,EMPTY);
    sem_wait(sem,MUTEX);
    add(buffer,i);
    sem_signal(sem,MUTEX);
    sem_signal(sem,FULL);
  }

  /* send a -1 to signal the end of the stream */

  sem_wait(sem,EMPTY);
  sem_wait(sem,MUTEX);
  add(buffer, -1);
  sem_signal(sem,MUTEX);
  sem_signal(sem,FULL);
  
  shm_detach((char *)buffer);
}

void add(int *buffer, int val)
{
  buffer[buffer[HEAD]]=val;
  buffer[HEAD]=(buffer[HEAD]+1)%N;
}

void consumer(void)
{
  int eos=0, *buffer, val;

  buffer=(int *)shm_attach(memblk);

  while (!eos) {
    sem_wait(sem,FULL);
    sem_wait(sem,MUTEX);
    val=del(buffer);
    sem_signal(sem,MUTEX);
    sem_signal(sem,EMPTY);

    printf("%d\n",val);
    eos=(val==-1);
  }

  shm_detach((char *)buffer);
}
  
int del(int *buffer)
{
  int val;

  val=buffer[buffer[TAIL]];
  buffer[TAIL]=(buffer[TAIL]+1)%N;
  return val;
}

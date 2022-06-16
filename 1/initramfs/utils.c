#include "utils.h"
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <time.h>
#include <stdlib.h>


union semun {                   /* Used in calls to semctl() */
    int                 val;
    struct semid_ds *   buf;
    unsigned short *    array;
    struct seminfo *    __buf;
};

static int semid;

void enter_region(char *name) {
  struct sembuf sop = {0, -1, 0};

  if (semop(semid, &sop, 1) == -1)
      perror("semop");
  printf("# Trying to enter in critical region as %s\n", name);
  puts("-----------------------------------------------------");
}

void leave_region(char *name) {
  struct sembuf sop = {0, 1, 0};


  if (semop(semid, &sop, 1) == -1)
      perror("semop");

  printf("# Leaving critical region as %s\n", name);
  puts("-----------------------------------------------------");
}

void init_semaphore() {
  static int initialized = 0;
  key_t key;
  union semun arg;

  if (initialized)
    return;

  key = ftok("init", 'p');
  if (key == -1) {
      perror("ftok");
      exit(-1);
  }

  semid = semget(key, 2, IPC_CREAT | 0600);
  if (semid == -1) {
      perror("semget");
      exit(-1);
  }

  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1) {
      perror("semctl");
      exit(-1);
  }

  initialized = 1;
}

void print_message(char* name) {
  // setvbuf(stdout, NULL, _IONBF, 0);  
  while(1) {
    printf("# In process %s\n", name);
    puts("\n");
    enter_region(name);
    sleep(1.5);
    printf("Inside critial region as %s\n", name);
    sleep(1.5);
    printf("Still inside critial region as %s\n", name);
    sleep(1.5);
    printf("Still inside critial region as %s\n", name);
    sleep(10);
    leave_region(name);
  }
}
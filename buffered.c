#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MSG_COUNT 5

int main(int argc, char const *argv[]) {

  int proc_count;
  int curr_proc;
  srand(time(NULL) + curr_proc);
  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &curr_proc);

  if(curr_proc == 0) {
    int sizes[MSG_COUNT];
    int totalsize = 0;
    int bufsize;
    char *sendingStuff[] = {
      "Bardzo dlugi string",
      "O, a ja jestem dluzszy",
      "A to ciekawe, co mowicie, koledzy",
      "Samplesamplesample",
      "qwerty"
    };
    for(int i = 0; i < MSG_COUNT; i++) {
      MPI_Pack_size(strlen(sendingStuff[i]), MPI_CHAR, MPI_COMM_WORLD, &sizes[i]);
      totalsize += sizes[i];
    }

    bufsize =  (proc_count - 1 ) * MPI_BSEND_OVERHEAD + totalsize;
    char * buf =  (char *)malloc(bufsize);

    printf("Total size: %d\n", bufsize);

    MPI_Buffer_attach(buf, bufsize);

    for(int i = 0; i < MSG_COUNT; i++) {
        for(int j = 1; j < proc_count; j++) {
          MPI_Bsend(&sendingStuff[i], strlen(sendingStuff[i]), MPI_CHAR, j, 0, MPI_COMM_WORLD);
      }
    }

    MPI_Buffer_detach(&buf, &bufsize);

  } else if(curr_proc != 0) {
    int current_str_len;
    MPI_Status stat;
    char * incoming_msg = NULL;
    for(int i = 0; i < MSG_COUNT; i++) {

      MPI_Probe(0, 0, MPI_COMM_WORLD, &stat);
      MPI_Get_count(&stat, MPI_CHAR, &current_str_len);

      incoming_msg = (char*)malloc(sizeof(char) * current_str_len);

      MPI_Recv(&incoming_msg, current_str_len, MPI_CHAR, 0, 0 , MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
      printf("Proc %d at step %d received \'%s\'\n",curr_proc, i, incoming_msg);
      fflush(stdout);
      sleep( (rand() / (float)RAND_MAX) * 10 + 1 );
    }
  }

  MPI_Finalize();
  return 0;
}

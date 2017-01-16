#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_COUNT 10

int main(int argc, char const *argv[]) {

  int proc_count;
  int curr_proc;

  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &curr_proc);


  if(curr_proc == 0) {
    char sendingStuff[MSG_COUNT] = {'a','b','c','d','e','f','g','h','i','j'};
    int sizes[MSG_COUNT];
    int bufsize;

    // for(int i = 0; i < MSG_COUNT; i++) {
    //   MPI_Pack_size(1, MPI_CHAR, MPI_COMM_WORLD, &sizes[i]);
    // }
    //int packsize = sizeof(char) * 1 * MSG_COUNT; // typ * ilosc wiadomosci * ile znakow na wiadomosci
    bufsize =  MSG_COUNT * MPI_BSEND_OVERHEAD + sizeof(char) * MSG_COUNT;
    char * buf =  (char *)malloc(bufsize);
    MPI_Buffer_attach(buf, bufsize);

    for(int i = 0; i < MSG_COUNT; i++) {
      MPI_Bsend(&sendingStuff[i], 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }
    MPI_Buffer_detach(&buf, &bufsize);

  } else if(curr_proc == 1) {
    char receivingStuff[MSG_COUNT];
    for(int i = 0; i < MSG_COUNT; i++) {

      MPI_Recv(&receivingStuff[i], 1, MPI_CHAR, 0, 0 , MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
      printf("At step %d received %c\n", i, receivingStuff[i]);
      fflush(stdout);
      sleep(2);
    }

  }


  MPI_Finalize();

  return 0;
}

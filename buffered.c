#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MSG_COUNT 10

int main(int argc, char const *argv[]) {

  int proc_count;
  int curr_proc;
  srand(time(NULL));
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
    bufsize =  (proc_count - 1 ) * MPI_BSEND_OVERHEAD + sizeof(char) * MSG_COUNT;
    char * buf =  (char *)malloc(bufsize);

    MPI_Buffer_attach(buf, bufsize);

    for(int i = 0; i < MSG_COUNT; i++) {
        for(int j = 1; j < proc_count; j++) {
          MPI_Bsend(&sendingStuff[i], 1, MPI_CHAR, j, 0, MPI_COMM_WORLD);
      }
    }

    MPI_Buffer_detach(&buf, &bufsize);



  } else if(curr_proc != 0) {
    char receivingStuff[MSG_COUNT];
    for(int i = 0; i < MSG_COUNT; i++) {

      MPI_Recv(&receivingStuff[i], 1, MPI_CHAR, 0, 0 , MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
      printf("Proc %d at step %d received %c\n",curr_proc, i, receivingStuff[i]);
      fflush(stdout);
      sleep( (rand() / (float)RAND_MAX) * 10 + 1 );
    }

  }


  MPI_Finalize();

  return 0;
}

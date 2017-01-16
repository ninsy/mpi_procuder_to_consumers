#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {

  srand(time(NULL));
  for (int i = 0; i < 20; i++) {
    int num = (rand() / (float)RAND_MAX) * 10 + 1;
    printf("Random num: %d\n", num);
  }


  return 0;
}

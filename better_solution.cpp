#include <mpi.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <vector>

#define MAX_NUMBERS 5

using namespace std;

enum TAG_TYPE { KILL, CONTAINS };
int proc_count;
int curr_proc;

void sendKillMessage(const vector<int>& proccesses) {
  int fakeMsg = 1;
  for(int procNumber : proccesses) {
    MPI_Send(&fakeMsg, 1, MPI_INT, procNumber, TAG_TYPE::KILL, MPI_COMM_WORLD);
  }
}

void fetchMetadata(int source, int &tag, int &size) {
  MPI_Status stat;
  MPI_Probe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  MPI_Get_count(&stat, MPI_INT, &size);
  tag = stat.MPI_TAG;
}

void receiveMessages(vector<int>& v, int source, int tag, int messageCount) {
  cout << "Process [ " << curr_proc << " ] is going to receive " << messageCount << " messages" << endl;
  v.resize(messageCount);
  MPI_Recv(&v[0], v.size(), MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
}

void producerActiveLoop(int &itemCnt, vector<int> &i) {
  while (true) {
    itemCnt = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
    if(!itemCnt) {
      vector<int> targets = {1};
      sendKillMessage(targets);
      break;
    } else {
      i.reserve(itemCnt);
      cout << "Producer is going to send: [ ";
      for(int j = 0; j < itemCnt; j++) {
        i.push_back(rand()%10);
        cout << i[j] << " ";
      }
      cout << "]." << endl;
      MPI_Send((void*)i.data(), itemCnt, MPI_INT, 1, TAG_TYPE::CONTAINS, MPI_COMM_WORLD);
      i.clear();
    }
  }
}

void bufferActiveLoop(int &itemCnt, vector<int> &i) {
  int tag = TAG_TYPE::CONTAINS;
  queue<int> messages;
  while(tag != TAG_TYPE::KILL || messages.size()) {

    if(tag != TAG_TYPE::KILL) {
      fetchMetadata(0, tag, itemCnt);
      if(tag != TAG_TYPE::KILL && itemCnt) {
        i.resize(itemCnt);
        MPI_Recv(&i[0], i.size(), MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int item : i) {
          messages.push(item);
        }
        i.clear();
      }
    }
    if(!messages.empty()) {
      int currentMessage = messages.front();
      for(int i = 2; i < proc_count; i++) {
        MPI_Send(&currentMessage, 1, MPI_INT, i , TAG_TYPE::CONTAINS, MPI_COMM_WORLD);
      }
      messages.pop();
    }
  }
  for(int i = 2; i < proc_count; i++) {
    int fake = 1;
    MPI_Send(&fake, 1, MPI_INT, i , TAG_TYPE::KILL, MPI_COMM_WORLD);
  }
}

void consumerActiveLoop(int &itemCtn, vector<int> &i) {
  int tag = TAG_TYPE::CONTAINS;
  while(tag != TAG_TYPE::KILL) {
    fetchMetadata(1, tag, itemCtn);
    if(tag != TAG_TYPE::KILL) {
      i.resize(itemCtn);
      MPI_Recv(&i[0], i.size(), MPI_INT, 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      cout << "Process [" << curr_proc << "] received items: [ ";
      for(int item : i) {
        cout << item << " ";
      }
      cout << "]." << endl;
      i.clear();
      sleep( (rand() / (float)RAND_MAX) * 10 + 1 );
    }
  }
}

int main(int argc, char const *argv[]) {
  srand(time(NULL) + curr_proc);
  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &curr_proc);

  vector<int>items;
  int itemCount;

  if(curr_proc == 0) {
    producerActiveLoop(itemCount, items);
  } else if(curr_proc == 1) {
    bufferActiveLoop(itemCount, items);
  } else {
    consumerActiveLoop(itemCount, items);
  }

  MPI_Finalize();
  return 0;
}

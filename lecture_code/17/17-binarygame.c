#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CLEARSCREEN "\033[2J"
#define MOVETO "\033[%d;%d;H"
#define BKCOLOR"\033[%dm"
#define BKBLACK "\033[40m"

#define N (8)

pthread_t tids[N], display_tid;
pthread_cond_t cvs[N];
pthread_mutex_t locks[N];
char* status[N+1];
int data[N+1];
int quit;

void* display_runner(void*arg) {
  while(!quit) { // memory fence?
    printf(CLEARSCREEN MOVETO , 1, 1); // clear screen then move to top-left
    for(int i = 0; i < N; i++) {
      printf("%02i %5s %3i ", i, status[i], data[i]);
      printf( BKCOLOR,41+ (i%7));
      for(int j=data[i]>64?64:data[i];j--;) putchar('*');
      printf( BKBLACK "\n");
    }
    usleep(1E5);
  }
  return NULL;
}
void init() {
  for(int i =0; i < N;i++) {
   
    pthread_cond_init(cvs + i, NULL);
    pthread_mutex_init(locks + i, NULL);
    status[i] = "---";
  }
  pthread_create( &display_tid,NULL, display_runner, NULL);
}
void update_display(int row, char* mesg) {
  if(row>=0 && row<N && mesg)
     status[row] = mesg;
}

 
// Waits until data[i] > 1, then half it and increments data[i+1]
void* runner(void*arg) {
   int i = (int) arg; // slightly evil / fragile
   while(1) {
     update_display(i,"Lok");
     pthread_mutex_lock(locks+i);

     while(!quit && data[i] < 2 ) { 
       update_display(i,"Nap");
       pthread_cond_wait(cvs + i,locks+i);
     }
     if(quit) break;

     data[i] -= 2; 
     
       update_display(i,"L+1");
       pthread_mutex_lock(locks+i+1);
       data[i+1] ++;
       pthread_mutex_unlock(locks+i+1);
       pthread_cond_broadcast( cvs+i+1);
     
     pthread_mutex_unlock(locks+i);
     update_display(i,"Yum" );
     usleep(2E5);
   }
   update_display(i,"Bye");
}

void modify(int index, int amount) {
  if(index<0 || index>=N) return;
  pthread_mutex_lock(locks + index);
  data[index] += amount;
  pthread_cond_broadcast( cvs + index);
  pthread_mutex_unlock(locks + index);

  update_display(index,"Add");
}

void start_threads() {
  for(int i=0; i <N-1;i++)
    pthread_create(tids+i, NULL, runner, (void*)i);
}

int main() {
  init();
  data[0] = 1 << (N-1);
  start_threads();
  while(1) { 
    int c = getchar();
    if(c=='q' || c == EOF) break;
    int index = c - '0';
    if(index>=0 && index < N) modify(index, 8);
  }
  puts("Quitting");
  quit = 1;
  pthread_exit(NULL);
}

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Prep: gcc takeovertheworld.c -o takeovertheworld
// Demo: ./a.out ./takeovertheworld 13 
// also discuss nohup command &

void quit(char*mesg) { perror(mesg); exit(1); }

int main(int argc, char**argv) {
    if(argc <2) {fprintf(stderr,"Usage: %s command args\n", argv[0]); exit(1);}
    puts("Press CTRL-C as much as you want...");

    sigset_t mask;
    sigset_t orig_mask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGINT);
    puts("SIG_BLOCKing SIGINT"); 
    sigprocmask(SIG_BLOCK, &mask, &orig_mask);
    
    printf("exec %s ...\n", argv[1]);
    execvp(argv[1],argv+1);

    quit("exec failed");
    return 1;
}

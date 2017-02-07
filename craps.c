/**
 * Game of luck: Implementation of the Gamemaster
 *
 * Course: Operating Systems and Multicore Programming - OSM lab
 * assignment 1: game of luck.
 *
 * Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *
 * History
 *
 * 2016-01-31 (Mahdad Davari)
 *
 * Added more annotations and fine-grained TODO lists 
 *
 */
#include <errno.h>
#include <stdio.h> /* I/O functions: printf() ... */
#include <stdlib.h> /* rand(), srand() */
#include <unistd.h> /* read(), write() calls */
#include <assert.h> /* assert() */
#include <time.h>   /* time() */
#include <signal.h> /* kill(), raise() and SIG???? */

#include <sys/types.h> /* pid */
#include <sys/wait.h> /* waitpid() */

#include "common.h"

int main(int argc, char *argv[])
{
	int i, seed;

	// TODO 1: Un-comment the following variables to use them in the 
	//         exec system call. Using the function sprintf and the arg1 
	//         variable you can pass the id parameter to the children 

	char arg0[] = "./shooter";
	/* char arg1[NUM_PLAYERS]; */
  char arg1[10];
	char *args[] = {arg0, arg1, NULL};
	

	// TODO 2: Declare pipe variables
	//         - Of which data type should they be?
	//         - How many pipes are needed?
	//         - Try to choose self-explanatory variable names, e.g. seedPipe, scorePipe
  int seed_pipes[NUM_PLAYERS][2];
  int score_pipes[NUM_PLAYERS][2];
  int children_pids[NUM_PLAYERS];
	// TODO 3: initialize the communication with the players, i.e. create the pipes
  int pipeMsg = 0;
  for (i = 0; i < NUM_PLAYERS; i++) {
    pipeMsg = pipe(seed_pipes[i]);
    if(pipeMsg != 0)
      printf("bug when opening pipe: %i\n", pipeMsg);
    pipe(score_pipes[i]);
    if(pipeMsg != 0)
      printf("bug when opening pipe: %i\n", pipeMsg);
    /* seed_pipes[i][0] = 3; */
    /* seed_pipes[i][1] = 4; */
    /* score_pipes[i][0] = 3; */
    /* score_pipes[i][1] = 4; */

	}

	// TODO 4: spawn/fork the processes that simulate the players
	//         - check if players were successfully spawned
	//         - is it needed to store the pid of the players? Which data structure to use for this?
	//         - re-direct standard-inputs/-outputs of the players
	//         - use execv to start the players
	//         - pass arguments using args and sprintf

	for (i = 0; i < NUM_PLAYERS; i++) {
    children_pids[i] = fork();
    assert(children_pids[i]>=0);
    sprintf(arg1, "%d", i);
    if(children_pids[i] == 0){
      for(int j = 0; j<NUM_PLAYERS; ++j){
        close(seed_pipes[j][1]);
        close(score_pipes[j][0]);
        if(i != j){
          close(score_pipes[j][1]);
          close(seed_pipes[j][0]);
        }
        dup2(seed_pipes[i][0], STDIN_FILENO);
        dup2(score_pipes[i][1], STDOUT_FILENO);
        int execReturn = execv(arg0, args);
        printf("Execreturnvalue:%i ", execReturn);
      }
      /* shooter(i, seed_pipes[i][0], score_pipes[i][1]); */
    }
  }

  for(int j = 0; j<NUM_PLAYERS; ++j){
    close(seed_pipes[j][0]);
    close(score_pipes[j][1]);
  }

	seed = time(NULL);

  
	for (i = 0; i < NUM_PLAYERS; i++) {
		seed++;
    /* printf("Seed in craps: %i\n", seed); */

    // TODO 5: send the seed to the players (write using pipes)
    int writeResult = write(seed_pipes[i][1], &seed, sizeof(int));
    if(writeResult <=0 ){
      perror("write() error 104");
    }
    /* printf("Write: %i\n", writeResult); */
    /* perror("write() failed"); */
  }


	// TODO 6: read the dice results from the players via pipes, find the winner

  int maxScore = 0, score, winner;
	for (i = 0; i < NUM_PLAYERS; i++) {
    read(score_pipes[i][0], &score, sizeof(int));
    /* printf("Score in craps.c:%i, process: %i \n", score, i); */
    if (maxScore<score){
      maxScore = score;
      winner = i;
    }
  }

	printf("master: player %d WINS with a score of: %i \n",winner, maxScore);


	// TODO 7: signal the winner
	//         - which command do you use to send signals?
	//         - you will need the pid of the winner
  kill(children_pids[winner], SIGUSR1);
	

	// TODO 8: signal all players the end of game
	//         - you will need the pid of all the players

	for (i = 0; i < NUM_PLAYERS; i++) {
    if(i != winner){
      kill(children_pids[i], SIGUSR2);
      /* printf("IGETHEREHHELLOOOO pid: %i\n", children_pids[i]); */
    }
  }


	printf("master: the game ends\n");


	// TODO 9: cleanup resources and exit with success
	//         wait for all the players/children to exit
	//         before game master exits 

	for (i = 0; i < NUM_PLAYERS; i++) {
    int exitValue = 0;
    wait(&exitValue);
	}

	return 0;
}

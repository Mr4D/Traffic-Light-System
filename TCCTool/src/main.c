#include "main.h"

typedef struct
{
	int aflag;
	int bflag;
} tool_data;

int main(int argc,char *argv[])
{

	  tool_data tool_data;
	  char *arg1;
	  char *arg2;



	  if (argc !=3 || argc != 4)
	  {
		  printf("Possible Arguments:\n");
	  	  printf("\t[g]\tGet Information\n\n");
	  	  printf("\t\t[m]ode\n\n");
	  	  printf("\t\t[s]tate\n");
	  	  printf("\t[s]\tSend Information\n\n");
	  	  printf("\t\t[m]ode\t[0,1]\n");
	  	  printf("\t\t[r]eset\n");
	  	  printf("\t\t[f]ault\n\n");
	  	  printf("Example: 'g state', 's mode'");
	  	  return 0;
	  }

		  arg1 = argv[1];
	  	  arg2 = argv[2];

	  	  printf("Debug1 | Option: %s %s",arg1,arg2);

	  	  //Use a switch statement next time, dunce.

	  	  if (arg1[0] == 'g' ||arg1[0] == 'G')
	  		  tool_data.aflag = 1;
	  	  else if (arg1[0] == 's' ||arg1[0] == 'S')
	  		  tool_data.aflag = 2;
	  	  else
	  	  {
	  		  printf("Illegal First Option Selected.");
	  	  	  return 0;
	  	  }

	  	  if (arg2[0]== 'm' || arg2[0]== 'M')
	  		  tool_data.bflag = 1;
	  	  else if (arg2[0] == 'r' ||arg2[0] == 'R')
	  		  tool_data.bflag = 2;
	  	  else if (arg2[0] == 'f' ||arg2[0] == 'F')
	  		  tool_data.bflag = 3;
	  	  else
	  	  {
	  		  printf("Illegal Second Option Selected");
	  		  return 0;
	  	  }

	  	  pid_t processID;
	  	  pthread_t input;

	  	  char hostname[MAXHOSTNAMELEN];
	  	  if(gethostname(hostname, sizeof(hostname)) == -1)
	  	  {
	  		  printf("gethostname() failed\n");
	  	  }

	  	  processID = getpid();

	  	  void * retval;

	  	  pthread_create (&input, NULL, input_thread, &tool_data);
	  	  pthread_join(input, &retval);

	  	  return 0;
}

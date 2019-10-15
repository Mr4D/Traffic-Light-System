#include "main.h"

typedef struct
{
	int intersection;
	int aflag;
	int bflag;
	int cflag;
} tool_data;

int main(int argc,char *argv[])
{

	  tool_data tool_data = {0,0,0,0};

	  char *arg1;
	  char *arg2;



	  if (argc !=4 || argc != 5)
	  {
		  printf("Possible Arguments:\n");
		  printf("[a] Intersection (I1)\n");
		  printf("[b] Intersection (I2)\n\n");
	  	  printf("\t[g]\tGet Information\n\n");
	  	  printf("\t\t[m]ode\n\n");
	  	  printf("\t\t[s]tate\n");
	  	  printf("\t[s]\tSend Information\n\n");
	  	  printf("\t\t[m]ode\t[p]eak,[o]ffpeak\n");
	  	  printf("\t\t[r]eset\n");
	  	  printf("\t\t[f]ault\n\n");
	  	  printf("Example: 'a g state'\n  'B2 Setz mode PeAk' is equivalent to 'b s m p'");
	  	  return 0;
	  }

	  arg1 = argv[1];
	  arg2 = argv[2];
	  arg3 = argv[3];
	  arg4 = argv[4];

	  printf("Debug1 | Option: %s %s",arg1,arg2);

	  //Use a switch statement next time, dunce.
	  switch(arg1[0])
		  case 'a':
			  tool_data.intersection = 0;
			  break;
		  case 'A':\
		  	  tool_data.intersection = 0;
		  	  break;
		  case 'b':
			  tool_data.intersection= 1;
		  	  break;
		  case 'B':
			  tool_data.intersection= 1;
		  	  break;
		  default:
			  printf("Illegal  selection");
			  return 0;

	  if (arg2[0] == 'g' ||arg2[0] == 'G')
		  tool_data.aflag = 1;
	  else if (arg2[0] == 's' ||ar2[0] == 'S')
		  tool_data.aflag = 2;
	  else
	  {
		  printf("Illegal Get/Set Option Selected.\n");
		  return 0;
	  }

	  if (arg3[0]== 'm' || arg3[0]== 'M')
		  tool_data.bflag = 1;
	  else if (arg3[0] == 'r' ||arg3[0] == 'R')
		  tool_data.bflag = 2;
	  else if (arg3[0] == 'f' ||arg3[0] == 'F')
		  tool_data.bflag = 3;
	  else
	  {
		  printf("Illegal third Option Selected.\n");
		  return 0;
	  }
	  if (argc == 5)
	  {
		  switch(arg4[0])
		  case 'p':
			  tool_data.cflag = 0;
			  break;
		  case 'P':\
		  	  tool_data.cflag = 0;
		  	  break;
		  case 'o':
			  tool_data.cflag = 1;
		  	  break;
		  case 'O':
			  tool_data.cflag = 1;
		  	  break;
		  default:
			  printf("Illegal Peak/Offpeak selection.\n");
			  return 0;

	  }

	  printf("\nIntersection: %d, Get or Set: %d")
	  pid_t processID;
	  pthread_t input;

	  char hostname[MAXHOSTNAMELEN];
	  if(gethostname(hostname, sizeof(hostname)) == -1)
	  {
		  printf("gethostname() failed\n");
	  }

	  processID = getpid();

	  void * retval;

	  pthread_create (&input, NULL, &client_input_thread, &tool_data);
	  pthread_join(input, &retval);

	  return 0;
}

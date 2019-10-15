#include "input.h"



int main(int argc,char *argv[])
{

	  tool_data_t tools = {0,0,0,0};

	  char *arg1;
	  char *arg2;
	  char *arg3;
	  char *arg4;


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
	  {
		  case 'a':
			  tools.intersection = 0;
			  break;
		  case 'A':
			  tools.intersection = 0;
		  	  break;
		  case 'b':
			  tools.intersection= 1;
		  	  break;
		  case 'B':
			  tools.intersection= 1;
		  	  break;
		  default:
			  printf("Illegal  selection");
			  return 0;
	  }

	  if (arg2[0] == 'g' ||arg2[0] == 'G')
		  tools.aflag = 1;
	  else if (arg2[0] == 's' ||arg2[0] == 'S')
		  tools.aflag = 2;
	  else
	  {
		  printf("Illegal Get/Set Option Selected.\n");
		  return 0;
	  }

	  if (arg3[0]== 'm' || arg3[0]== 'M')
		  tools.bflag = 1;
	  else if (arg3[0] == 'r' ||arg3[0] == 'R')
		  tools.bflag = 2;
	  else if (arg3[0] == 'f' ||arg3[0] == 'F')
		  tools.bflag = 3;
	  else
	  {
		  printf("Illegal third Option Selected.\n");
		  return 0;
	  }
	  if (argc == 5)
	  {
		  switch(arg4[0])
		  {
		  case 'p':
			  tools.cflag = 0;
			  break;
		  case 'P':\
		  	  tools.cflag = 0;
		  	  break;
		  case 'o':
			  tools.cflag = 1;
		  	  break;
		  case 'O':
			  tools.cflag = 1;
		  	  break;
		  default:
			  printf("Illegal Peak/Offpeak selection.\n");
			  return 0;
		  }

	  }

	  printf("\nIntersection: %d, Get or Set: %d");

	  pid_t processID;
	  pthread_t input;

	  char hostname[MAXHOSTNAMELEN];
	  if(gethostname(hostname, sizeof(hostname)) == -1)
	  {
		  printf("gethostname() failed\n");
	  }

	  processID = getpid();

	  void * retval;

	  pthread_create (&input, NULL, &client_input_thread, &tools);
	  pthread_join(input, &retval);

	  return 0;
}

#ifndef MAPRED_H
#define MAPRED_H
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

extern bool user;
extern bool machine;
extern char * currentUserColor;
extern char * currentMachineColor;
extern char * shellDirectory;
extern char * shellName;
extern char * chpmtString;
extern char * newShellName;
extern char * currentWorkingDirectory;
extern char * lastWorkingDirectory;
extern char **environ;
extern char**newArgV;;
extern int newArgC;
extern size_t lastReturnValue;

extern bool redirectionPipe;
extern bool redirectionLessThan;
extern bool redirectionMoreThan;
extern bool redirectionPresent;
extern int  * redirectionIndex;

extern char * argVWithSpaces;

extern int IN_FD;
extern int OUT_FD;

extern int pipes;

extern char *cmd;
extern char * buffer;
extern bool backgroundTask;

extern int processCount;
extern int status;

extern int currentFD;

extern int SPID;
extern int processCount;

void print_help();
void changeDirectory(char * string);
void getpwdHelper(char * shellDirectory);
char * chpmtHelper(char * shellName, char* stringToConcat, char * SETTING , char * TOGGLE);
char * chclrHelper(char * shellName, char* stringToConcat, char * TOGGLE , char * SETTING , char * COLOR , char * BOLD);
char * changeShellSettings(char * newShellName, char* shellName , char * shellDirectory, char * currentWorkingDirectory);
int checkBuiltIn(char ** userInput);

char ** parseArgs(char*cmd);
void fileFinder(char** newArgV);
bool fileExists(const char* file);
void redirectionIndexFinder();

char * getCommand(char * currentCommand);
char ** getRHS();
char ** getLHS();

void pipingExecute(char ** command , int pipes);
char ** buildExecutable(char ** arg);


void redirectionParse(char * command);
void redirectionExecute(char ** arg);

char ** tokenizeSpaces(char * arg);

typedef struct job {
	char * jobName;
	int JID;
	int PID;
	int GID;
	int process;
	char * status;
	bool fg;

	struct job * next;
}job;
extern job * head;

void addNode(char * jobName, int PID, int GID, int process, char * status);
job * iterator(job * cursor);
void printer(job * cursor);
job * getTail(job * cursor);
bool removeNode(int pid);
job * nodeFinder(pid_t pid);
job * getPreviousNode(job * child);
bool pidChecker(pid_t pid);

void handleChild(int sig);
int getPID(char ** userInput);
void switchToFG(pid_t pid);
void switchToBG(pid_t pid);
void handleInterruption(int sig);
void printSFISH();
#endif


// node list [128];
// 	int count = 0;
// 	bool breakLoop = true;
// 	do{//grep .c < out.txt > in.txt | ls -l > out2.txt
// 		printf("%s\n", "I enter here");
// 		list[count].command = NULL;
// 		list[count].in = NULL;
// 		list[count].out = NULL;
// 		int i = 0;
// 		for(i = 0; i < newArgC ; i ++){
// 			if(strcmp(newArgV[i],">")==0){
// 				i++;
// 				char * string = calloc(1024,sizeof(char));
// 				strcpy(string,newArgV[i]);
// 				list[count].out = string;
// 			}else if(strcmp(newArgV[i],"<")==0){
// 				i++;
// 				char * string = calloc(1024,sizeof(char));
// 				strcpy(string,newArgV[i]);
// 				list[count].in = string;
// 			}else if(strcmp(newArgV[i],"2>")==0){
// 				i++;
// 				char * string = calloc(1024,sizeof(char));
// 				strcpy(string,newArgV[i]);
// 				list[count].out = string;
// 			}else if(strcmp(newArgV[i],"|")==0){
// 				break;
// 			}else if(newArgV[i] == NULL){
// 				breakLoop = false;
// 			}
// 			char * string = calloc(1024,sizeof(char));
// 			strcpy(string,newArgV[i]);
// 			list[count].command[i] = string;
// 		}
// 		i++;			
// 		list[count].command[i] = NULL;
// 		count++;
// 	}while(breakLoop);
// 	printf("%s\n", "I enter here");
// 	for(int i = 0 ; i < count; i++){
// 		printf("%d\n", i);
// 		printf("list[count].in : %s\n", list[count].in);
// 		printf("list[count].out: %s\n", list[count].out);
// 		int count = 0;
// 		do{
// 			printf("list[count].command[count] : %s\n",list[count].command[count] );	
// 		}while(list[count].command[count]!=NULL);
// 	}














// int currentRedirectionIndex = 0;
// int currentCommandIndex = 0;
// char ** arguementArray = NULL;

// void noRedirectionHandler(){
// 	char ** lhs = calloc(4096, sizeof(char));
// 	lhs = getLHS();

// 	char* currentCommand = calloc(1024, sizeof(char));
// 	if(getCommand(lhs[0])!=NULL){
// 		strcpy(currentCommand,getCommand(lhs[0]));//First index of lhs is always the command
// 	}
// 	else{
// 		printf("%s: command not found\n", currentCommand);
// 		return;
// 	}
// 	pid_t  pid = fork();
// 	if(pid ==0){
// 		printf("%s\n", "====CHILD====");
// 		// for(int i =0 ; i < 10; i ++){
// 		// 	printf("Index : %s\n", lhs[i]);
// 		// 	if(lhs[i]==NULL){
// 		// 		break;
// 		// 	}
// 		// }
// 		// printf("%s\n", currentCommand);
// 		execvp(currentCommand,lhs);// we pass in the entire string with null terminated 
// 		exit(0);//kills the child
// 	}
// 	else{
// 		waitpid(pid,NULL,0);
// 		printf("%s\n", "====PARENT====");
// 	}
// }

// void rediectionMultipleHandler(){

// 	char ** rhsI = calloc(4096, sizeof(char));
// 	char ** rhsO = calloc(4096, sizeof(char));
// 	char ** lhs = calloc(4096, sizeof(char));

// 	lhs = getLHS();
// 	rhsI = getRHS();//first gets the input
// 	rhsO = getRHS();//second gets the output
// 	// return;
// 	// printf("rhs[0] : %s\n", rhs[0]);
// 	OUT_FD = open(rhsO[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); // First index of rhs is the file
// 	IN_FD = open(rhsI[0], O_RDONLY); // First index of rhs is the file
// 	if(IN_FD==-1){

// 	}
// 	char* currentCommand = calloc(1024, sizeof(char));
// 	strcpy(currentCommand,getCommand(lhs[0]));//First index of lhs is always the command

// 	pid_t  pid = fork();
// 	if(pid ==0){
// 		printf("%s\n", "====CHILD====");
// 		dup2(OUT_FD,STDOUT_FILENO);
// 		dup2(IN_FD,STDIN_FILENO);
// 		close(OUT_FD);
// 		close(IN_FD);
// 		// printf("currentCommand%s\n", currentCommand);

// 		execvp(currentCommand,lhs);// we pass in the entire string with null terminated 
// 	}
// 	else{
// 		waitpid(pid,NULL,0);
// 		printf("%s\n", "====PARENT====");
// 	}

// }

// void redirectionMoreThanHandler(){

// 	char ** rhs = calloc(4096, sizeof(char));
// 	char ** lhs = calloc(4096, sizeof(char));

// 	lhs = getLHS();
// 	rhs = getRHS();
// 	// printf("rhs[0] : %s\n", rhs[0]);
// 	OUT_FD = open(rhs[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); // First index of rhs is the file

// 	char* currentCommand = calloc(1024, sizeof(char));
// 	strcpy(currentCommand,getCommand(lhs[0]));//First index of lhs is always the command

// 	pid_t  pid = fork();
// 	if(pid ==0){
// 		printf("%s\n", "====CHILD====");
// 		dup2(OUT_FD,STDOUT_FILENO);
// 		close(OUT_FD);
// 		// printf("currentCommand%s\n", currentCommand);

// 		execvp(currentCommand,lhs);// we pass in the entire string with null terminated 
// 	}
// 	else{
// 		waitpid(pid,NULL,0);
// 		printf("%s\n", "====PARENT====");
// 	}
// }

// void redirectionLessThanHandler(){

// 	char ** rhs = calloc(4096, sizeof(char));
// 	char ** lhs = calloc(4096, sizeof(char));

// 	lhs = getLHS();
// 	rhs = getRHS();

// 	IN_FD = open(rhs[0], O_RDONLY); // First index of rhs is the file

// 	char* currentCommand = calloc(1024, sizeof(char));
// 	strcpy(currentCommand,getCommand(lhs[0]));//First index of lhs is always the command

// 	pid_t  pid = fork();
// 	if(pid ==0){
// 		printf("%s\n", "====CHILD====");
// 		dup2(IN_FD,STDIN_FILENO);
// 		close(IN_FD);
// 		execvp(currentCommand,lhs);// we pass in the entire string with null terminated 
// 	}
// 	else{
// 		waitpid(pid,NULL,0);
// 		printf("%s\n", "====PARENT====");
// 	}
// }

// char ** getLHS(){
// 	char ** lhs = calloc(4096,sizeof(char));
// 	int i =0;
// 	for(i = 0; i < newArgC ; i++){
// 		if(newArgV[currentPosLHS] == NULL){
// 			break;
// 		}

// 		if(strcmp(newArgV[currentPosLHS],">")==0 || strcmp(newArgV[currentPosLHS],"<")==0 || strcmp(newArgV[currentPosLHS],"2>")==0 || strcmp(newArgV[currentPosLHS],"|")==0){
// 			break;
// 		}
// 		else{
// 			char * string = calloc(128,sizeof(char));
// 			strcpy(string,newArgV[currentPosLHS]);
// 			lhs[i] = string;
// 		}
// 		currentPosLHS++;
// 	}
// 	i++;
// 	lhs[i] = NULL; // nullterinate the string
// 	printf("\n");
// 	for(int i =0; i < newArgC ; i ++){
// 		printf("lhs : %s\n", lhs[i]);
// 		if(lhs[i]==NULL){
// 			break;
// 		}
// 	}
// 	currentPosRedirection = currentPosLHS+1; // Setting where the current redirection is
// 	return lhs;
// }


// char ** getRHS(){
// 	printf("Carrert : %s\n", newArgV[currentPosRedirection-1]);
// 	currentPosRHS = currentPosRedirection;
// 	char ** rhs = calloc(4096,sizeof(char));
// 	int i =0;
// 	for(i = 0; i < newArgC ; i++){
// 		if(newArgV[currentPosRHS] == NULL){
// 			break;
// 		}

// 		if(strcmp(newArgV[currentPosRHS],">")==0 || strcmp(newArgV[currentPosRHS],"<")==0 || strcmp(newArgV[currentPosRHS],"2>")==0 || strcmp(newArgV[currentPosRHS],"|")==0){
// 			break;
// 		}
// 		else{
// 			char * string = calloc(128,sizeof(char));
// 			strcpy(string,newArgV[currentPosRHS]);
// 			rhs[i] = string;
// 		}
// 		currentPosRHS++;
// 	}
// 	i++;
// 	rhs[i] = NULL; // nullterinate the string
// 	printf("\n");
// 	for(int i =0; i < 10 ; i ++){
// 		printf("rhs : %s\n", rhs[i]);
// 		if(rhs[i]==NULL){
// 			break;
// 		}
// 	}
// 	currentPosRedirection = currentPosRHS+1;
// 	return rhs;
// }
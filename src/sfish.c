#include "sfish.h"

bool user = false;
bool machine = false;
char * currentUserColor = NULL;
char * currentMachineColor = NULL;
char * shellDirectory = NULL;
char * shellName = NULL;
char * chpmtString = NULL;
char * newShellName = NULL;
char * currentWorkingDirectory = NULL;
char * lastWorkingDirectory = NULL;

char**newArgV = NULL;
int newArgC = 0;
size_t lastReturnValue = 0;

bool redirectionPipe = false;
bool redirectionLessThan = false;
bool redirectionMoreThan = false;
bool redirectionPresent = false;


int  * redirectionIndex;

char * argVWithSpaces = NULL;

int IN_FD = STDIN_FILENO;
int OUT_FD = STDOUT_FILENO;

int pipes = 0;

char *cmd;
char * buffer;

bool backgroundTask = false;


job * head = NULL;

int status;

int currentFD;
int SPID = -1;
int processCount =0;

int ctrlH_Handler (int count, int key) {
	// printf ("key pressed: %d\n", key);
	printf("\n");
	print_help();
	rl_on_new_line();
	return 0;
}

int ctrlC_Handler (int count, int key){
	// job * node = calloc(sizeof(job),sizeof(char));
	// node = nodeFinder(currentFD);
	// int process = node->process;
	// for(int i = 0; i < process;i++){
	kill(-currentFD,SIGINT);
	addNode(buffer,currentFD,currentFD,0,"Stopped");
	backgroundTask = true;
	rl_on_new_line();
	return 0;
}

int ctrlZ_Handler (int count, int key){
	// job * node = calloc(sizeof(job),sizeof(char));
	// node = nodeFinder(currentFD);
	// int process = node->process;
	// for(int i = 0; i < process;i++){
	fprintf(stderr,"%d\n", currentFD);

	kill(currentFD,SIGTSTP);
	fprintf(stderr,"%d\n", currentFD);
	// 	currentFD++;
	// }
	rl_on_new_line();
	return 0;
}

int ctrlB_Handler (int count, int key){
	if(head==NULL){
		SPID = -1;
	}else{
		SPID = head->PID;
	}
	return 0;
}

int ctrlP_Handler(int count, int key){
	printf("\n");
	printSFISH();
	rl_on_new_line();
	return 0;
}

int ctrlG_Handler(int count, int key){
	printf("\n");
	if(head == NULL || SPID == -1){
		printf("%s\n", "SPID doesn't exist and has been set to -1");
	}else if(head->fg == true){//Foreground
		printf("[%d] %d stopped by signal %d\n", head->JID, head->PID,SIGSTOP);
		kill(-SPID,SIGSTOP);
	}else if(head->fg == false){//Background
		printf("[%d] %d stopped by signal %d\n", head->JID, head->PID,SIGTERM);
		kill(-SPID,SIGTERM);
	}
	rl_on_new_line();
	return 0;
}



int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
	rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.
	signal(SIGCHLD,handleChild);
	signal(SIGINT,handleInterruption);


	shellDirectory = calloc(1024,sizeof(char));
	shellName = calloc(1024,sizeof(char));
	chpmtString = calloc(1024, sizeof(char));
	newShellName = NULL;
	currentWorkingDirectory = calloc(1024, sizeof(char));
	currentUserColor = calloc(1024,sizeof(char));
	currentMachineColor = calloc(1024,sizeof(char));
	lastWorkingDirectory = calloc(1024,sizeof(char));
	char * hostname = calloc(1024,sizeof(char));
	redirectionIndex = calloc(1024,sizeof(char));

	rl_command_func_t ctrlH_Handler;
	rl_bind_keyseq("\\C-h", ctrlH_Handler);

	rl_command_func_t ctrlC_Handler;
	rl_bind_keyseq("\\C-c", ctrlC_Handler);

	rl_command_func_t ctrlB_Handler;
	rl_bind_keyseq("\\C-b", ctrlB_Handler);

	rl_command_func_t ctrlP_Handler;
	rl_bind_keyseq("\\C-p", ctrlP_Handler);

	rl_command_func_t ctrlZ_Handler;
	rl_bind_keyseq("\\C-z", ctrlZ_Handler);

	rl_command_func_t ctrlG_Handler;
	rl_bind_keyseq("\\C-g", ctrlG_Handler);

	cmd = calloc(1024,sizeof(char));
			// gethostname(hostname, 256);
	gethostname(hostname,1024);

	strcpy(shellName,"sfish:[");
	strcpy(currentUserColor,"\e[0m");
	strcpy(currentMachineColor,"\e[0m");//Starting from default color

	shellDirectory = strcat(shellName, getcwd(currentWorkingDirectory,1024));
	shellDirectory = strcat(shellDirectory,"]>");

	newShellName = calloc(1024,sizeof(char));
	newShellName = strcpy(newShellName,chpmtHelper("sfish", chpmtString , "1" , "user" ));
	shellDirectory = changeShellSettings(newShellName, shellName, shellDirectory, currentWorkingDirectory);
	// printf("%s\n", shellDirectory);

	free(newShellName);
	newShellName = calloc(1024,sizeof(char));
	newShellName = strcpy(newShellName,chpmtHelper("sfish", chpmtString , "1" , "machine" ));
	shellDirectory = changeShellSettings(newShellName, shellName, shellDirectory, currentWorkingDirectory+(strlen(getenv("HOME"))+1));

	while((cmd = readline(shellDirectory)) != NULL) {

		IN_FD = STDIN_FILENO;
		OUT_FD = STDOUT_FILENO;

		lastReturnValue = 0;

		redirectionPresent = false;
		backgroundTask = false;


		argVWithSpaces = calloc(1024,sizeof(char));
		if(cmd !=NULL && strcmp(cmd,"")!=0){
			processCount++;
			newArgV = parseArgs(cmd);
			// printf("backgroundTask : %d\n", backgroundTask);
			if (checkBuiltIn(tokenizeSpaces(newArgV[0]))==1){
				// printf("%s\n", "isBuiltIn");
			}else{
				fileFinder(newArgV);
			}
		}

		handleChild(0);
		SPID = -1; //Resetting the value of SPID
		// free(argVWithSpaces);
		shellDirectory = changeShellSettings(newShellName, shellName, shellDirectory, currentWorkingDirectory+(strlen(getenv("HOME"))+1));
	}

    //Don't forget to free allocated memory, and close file descriptors.
    //To return is not freed
	// free(cmd);
	// free(shellDirectory);
	// free(newShellName);
	// free(shellName);
    //WE WILL CHECK VALGRIND!

	return EXIT_SUCCESS;
}

char ** parseArgs(char*cmd){
	buffer = calloc(1024, ' ');
	int count = -1;
	for(int i = 0; i < 1024;i++){
		count++;
		buffer[count] = cmd[i];
		if(cmd[i]=='>'|| cmd[i]=='<' || cmd[i] == '|' || cmd[i]=='2'){
			if(cmd[i]=='>'|| cmd[i]=='<' || cmd[i] == '|'){//for other cases
				buffer[count]=' ';
				count++;//i+1
				buffer[count]=cmd[i];
				if(cmd[i]=='>'){
					redirectionMoreThan = true;
				}else if(cmd[i]=='<'){
					redirectionLessThan = true;
				}else{
					redirectionPipe = true;
				}
				redirectionPresent = true;
				count++;//i+2
				buffer[count]=' ';
			}
			else{// for 2> case
				if(cmd[i+1]=='>'){
					buffer[count]=' ';
					count++;//i+1
					buffer[count]=cmd[i];
					count++;//i+1
					i++;
					buffer[count]=cmd[i];
					count++;
					buffer[count]=' ';
					redirectionPresent = true;
				}
			}
		}
		if(cmd[i] == '\0'){
			count++;
			buffer[count] = '\0';
			break;
		}
	}

	// printf("%s\n", buffer);

	for(int i =strlen(buffer) -1; i >= 0; i--){
		if(buffer[i]==' '){
			continue;
		}
		else if(buffer[i]=='&'){
			buffer[i] = ' ';
			backgroundTask = true;
			break;
		}
		else{
			break;
		}
	}



	// printf("%s\n", buffer);

	char ** arrayToReturn = calloc(4096,sizeof(char));
	char * currentString = calloc(1024,sizeof(char));

	argVWithSpaces = strcpy(argVWithSpaces,buffer);//Keeping the string for future uses
	currentString = strcpy(currentString,buffer);

	char * currentStringCopy = calloc(1024,sizeof(char));
	currentStringCopy = strtok(currentString,"|");

	int index = 0;
	arrayToReturn[index] = currentStringCopy;

	while(currentStringCopy!=NULL){
		currentStringCopy = strtok(NULL,"|");
		index++;
		arrayToReturn[index] = currentStringCopy;

	}
	newArgC = index;

	for(int i = 0; i < 12 ; i ++){
		// printf("arrayToReturn[index] : %s\n", arrayToReturn[i]);
		if(arrayToReturn[i] == NULL){
			break;
		}
	}
	return arrayToReturn;

}

int checkBuiltIn(char ** userInput){
	int toReturn = 0;
	int status = 0;
	if(userInput[0]!=NULL){
		if(strcmp(userInput[0],"help")==0){
			if(!redirectionPresent){
				print_help();
			}
			else{
				// printf("%s\n", "I reach here");
				int fd = open(newArgV[(redirectionIndex[0])+1], O_RDWR|O_CREAT|O_TRUNC, 0600);
				// printf("%s\n", );
				pid_t pid =fork();
				if(pid==0){
					dup2(fd,STDOUT_FILENO);
					close(fd);
					print_help();
					exit(0);
				}else{
					currentFD = pid;
					waitpid(pid,&status,0);
				}
				close(fd);
			}
			toReturn = 1;
			lastReturnValue = status;
		}
		else if(strcmp(userInput[0],"exit")==0){
			exit(3);

			toReturn = 1;
		}
		else if(strcmp(userInput[0],"cd")==0){
		// printf("%s\n", "cd enterend");
		changeDirectory(userInput[1]);//Passing the string after the cmd
		toReturn = 1;
	}
	else if(strcmp(userInput[0],"pwd")==0){
			// printf("%s\n", "Pass");
		int status = 0;
		if(!redirectionPresent){
			getpwdHelper(shellDirectory);
		}
		else{
			int fd = open(newArgV[(redirectionIndex[0])+1], O_RDWR|O_CREAT|O_TRUNC, 0600);
					// printf("%s\n", );
			pid_t pid =fork();
			if(pid==0){
					// printf("%s\n", "I reach here");

				dup2(fd,STDOUT_FILENO);
				close(fd);
				getpwdHelper(shellDirectory);
				exit(0);
			}else{
				currentFD = pid;
				waitpid(pid,&status,0);
			}
			close(fd);
		}
		toReturn = 1;
		lastReturnValue = status;
	}
	else if(strcmp(userInput[0],"prt")==0){
		int fd = open(newArgV[(redirectionIndex[0])+1], O_RDWR|O_CREAT|O_TRUNC, 0600);

		pid_t pid = fork();
		if(pid==0){
			dup2(fd,STDOUT_FILENO);
			close(fd);
			printf("%lu\n", lastReturnValue);
		}else{
			currentFD = pid;
			waitpid(pid,&status,0);
		}
		close(fd);
		toReturn = 1;
	}
	else if(strcmp(userInput[0],"chpmt")==0){
			free(newShellName);//Clearing and reallocating space
			newShellName = calloc(1024, sizeof(char));
			strcpy(newShellName,chpmtHelper("sfish", chpmtString , userInput[2] , userInput[1] ));
			toReturn = 1;
		}
		else if(strcmp(userInput[0],"chclr")==0){
			free(newShellName);//Clearing and reallocating space
			newShellName = calloc(1024, sizeof(char));
			char * chclr = calloc(1024,sizeof(char));
			chclr = chclrHelper("sfish", chpmtString, 0 , userInput[3] , userInput[2] , userInput[1]);
			if(chclr!=NULL){
				strcpy(newShellName,chclr);
			}else{
				printf("%s\n", "Incorrect arguements provided");
			}

			toReturn = 1;
		}
		else if(strcmp(userInput[0],"jobs")==0){
			printer(head);//Prints out all the jobs
			toReturn = 1;
		}
		else if(strcmp(userInput[0],"fg")==0){
			int pid = getPID(userInput);
			if(pid!=-1){
				int numArgs = 0 ;
				while(userInput[numArgs]!=NULL){
					numArgs++;
				}
				if(numArgs == 2){
					switchToFG(pid);
				}
				else{
					printf("%s\n", "Incorrect arguements provided");
				}
			}
			else{
				printf("%s\n", "Incorrect arguements provided");
			}
			toReturn = 1;
		}
		else if(strcmp(userInput[0],"bg")==0){
			int pid = getPID(userInput);
			if(pid!=-1){
				int numArgs = 0 ;
				while(userInput[numArgs]!=NULL){
					numArgs++;
				}
				if(numArgs == 2){
					switchToBG(pid);
				}
				else{
					printf("%s\n", "Incorrect arguements provided");
				}
			}
			else{
				printf("%s\n", "Incorrect arguements provided");
			}
			toReturn = 1;
		}
		else if(strcmp(userInput[0],"kill")==0){
			int pid = getPID(userInput);
			job * node = nodeFinder(pid);
			// int count = node->process;
			if(pid!=-1){
				int numArgs = 0 ;
				while(userInput[numArgs]!=NULL){
					numArgs++;
				}
				if(numArgs == 2){
					if(pid!=-1){
						// printf("PID : %d\n", pid);
						// for(int i = 0; i < count; i++){
						kill(-pid,SIGTERM);
						// 	pid++;
						// }
						printf("[%d] %d stopped by signal 15\n",node->JID,pid);
					}

				}
				else if(numArgs == 3){
					if(pid!=-1){
						// for(int i = 0; i < count; i++){
						kill(-pid,atoi(userInput[1]));
						// 	pid++;
						// }
						printf("[%d] %d stopped by signal %d\n",node->JID, pid,atoi(userInput[1]));
					}
				}
				else{
					printf("%s\n", "Incorrect arguements provided");
				}
			}
			else{
				printf("%s\n", "Incorrect arguements provided");
			}
			toReturn = 1;
		}
		else if(strcmp(userInput[0],"disown")==0){
			int pid = getPID(userInput);
			if(pid!=-1){
				int numArgs = 0 ;
				while(userInput[numArgs]!=NULL){
					numArgs++;
				}
				if(numArgs == 2){
					removeNode(pid);
				}
				else if(numArgs == 1){
					head = NULL;
				}
				else{
					printf("%s\n", "Incorrect arguements provided");
				}
			}
			else{
				printf("%s\n", "Incorrect arguements provided");
			}
			toReturn = 1;
		}
		else{
			toReturn = 0;
		}	
	}	
	return toReturn;
}

char * changeShellSettings(char * newShellName, char* shellName , char * shellDirectory, char * currentWorkingDirectory){
	if(strstr(getcwd(currentWorkingDirectory,1024),getenv("HOME"))!=NULL){
		char * tempCWD = calloc(1024,sizeof(char));
		tempCWD = currentWorkingDirectory+(strlen(getenv("HOME"))+1);
		strcpy(currentWorkingDirectory, tempCWD);
	}
	if(newShellName == NULL){
		free(shellName);
		shellName = calloc(1024,sizeof(char));
		free(shellDirectory);
		shellDirectory = calloc(1024,sizeof(char));
		free(currentWorkingDirectory);
		currentWorkingDirectory = calloc(1024,sizeof(char));

		if(machine == false && user == false){
			strcpy(shellName,"sfish:[");
		}
		else{
			strcpy(shellName,"sfish-[");
		}

		if(strcmp(getcwd(currentWorkingDirectory,1024),getenv("HOME")) != 0){
			//if current working directory is NOT the home directory
			shellDirectory = strcat(shellName,getcwd(currentWorkingDirectory,1024));
			shellDirectory = strcat(shellDirectory,"]>");
		}
		else{
			//if current working directory is the home directory
			shellDirectory = strcat(shellName,"~");
			shellDirectory = strcat(shellDirectory,"]>");
		}
		// printf("Shell name 4 %s\n", shellName);
		// printf("shellDirectory : %s\n", shellDirectory);
	}
	else{
		free(shellName);
		shellName = calloc(1024,sizeof(char));
		free(shellDirectory);
		shellDirectory = calloc(1024,sizeof(char));

		strcpy(shellName,newShellName);

		char * checkCurrentDIR = calloc(1024,sizeof(char));
		if(strstr(getcwd(checkCurrentDIR,1024),getenv("HOME")) != NULL){
			if(strcmp(checkCurrentDIR,getenv("HOME"))==0){//Curent directory is home
				shellDirectory = strcat(shellName,"~");
			}
			else{
				shellDirectory = strcat(shellName,"~/");
				shellDirectory = strcat(shellName,currentWorkingDirectory);//This is a custom string without home
			}
			shellDirectory = strcat(shellDirectory,"]>");
		}
		else{
			//if current working directory is beyond home			
			shellDirectory = strcat(shellName,getcwd(currentWorkingDirectory,1024));
			shellDirectory = strcat(shellDirectory,"]>");
		}
	}
	return shellDirectory;
}

char * chclrHelper(char * shellName, char* stringToConcat, char * TOGGLE , char * BOLD , char * COLOR , char * SETTING){
	//DO THIS
	if(strcmp(SETTING,"user")==0){//Choosing if user is supposed to be kept in string
		if(strcmp(COLOR,"black")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;30m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;30m");
			}
		}
		else if(strcmp(COLOR,"red")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;31m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;31m");
			}
		}
		else if(strcmp(COLOR,"green")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;32m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;32m");
			}
		}
		else if(strcmp(COLOR,"yellow")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;33m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;33m");
			}
		}
		else if(strcmp(COLOR,"blue")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;34m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;34m");
			}
		}
		else if(strcmp(COLOR,"magenta")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;35m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;35m");
			}
		}
		else if(strcmp(COLOR,"cyan")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;36m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;36m");
			}
		}
		else if(strcmp(COLOR,"white")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[1;37m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentUserColor,'\0',1024);
				strcpy(currentUserColor,"\e[0;37m");
			}
		}

		if(user){
			return chpmtHelper(shellName,stringToConcat,"1" ,SETTING);
		}
		else{
			return chpmtHelper(shellName,stringToConcat,"0" ,SETTING);
		}
	}
	else if(strcmp(SETTING,"machine")==0){//Choosing if machine is supposed to be kept in string
		if(strcmp(COLOR,"black")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;30m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;30m");
			}
		}
		else if(strcmp(COLOR,"red")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;31m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;31m");
			}
		}
		else if(strcmp(COLOR,"green")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;32m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;32m");
			}
		}
		else if(strcmp(COLOR,"yellow")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;33m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;33m");
			}
		}
		else if(strcmp(COLOR,"blue")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;34m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;34m");
			}
		}
		else if(strcmp(COLOR,"magenta")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;35m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;35m");
			}
		}
		else if(strcmp(COLOR,"cyan")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;36m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;36m");
			}
		}
		else if(strcmp(COLOR,"white")==0){
			if(strcmp(BOLD,"1")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[1;37m");
			}
			else if(strcmp(BOLD,"0")==0){
				memset(currentMachineColor,'\0',1024);
				strcpy(currentMachineColor,"\e[0;37m");
			}
		}

		if(machine){
			return chpmtHelper(shellName,stringToConcat,"1" ,SETTING);
		}
		else{
			return chpmtHelper(shellName,stringToConcat,"0" ,SETTING);
		}
	}
	return NULL;

}

char * chpmtHelper(char * shellName, char* stringToConcat, char * TOGGLE , char * SETTING){
	// printf("SETTING : %s\n", SETTING );
	// printf("TOGGLE : %s\n", TOGGLE);
	// strcpy(currentUserColor,"\e[33m");
	// strcpy(currentMachineColor,"\e[33m");	
	if(strcmp(SETTING,"user")==0){//Choosing if user is supposed to be kept in string
		if(strcmp(TOGGLE,"1")==0){
			user = true;
		}
		else if(strcmp(TOGGLE,"0")==0){
			user = false;
		}
		else{
			lastReturnValue = EXIT_FAILURE; //Setting the exit value on failing
		}
	}
	else if(strcmp(SETTING,"machine")==0){//Choosing if machine is supposed to be kept in string
		if(strcmp(TOGGLE,"1")==0){
			// printf("%s\n", "Machine on");
			machine = true;
		}
		else if(strcmp(TOGGLE,"0")==0){
			machine = false;

		}
		else{
			lastReturnValue = EXIT_FAILURE; //Setting the exit value on failing
		}
	}
	else{
		lastReturnValue = EXIT_FAILURE; //Setting the exit value on failing
	}
	free(stringToConcat);//emptying the string
	stringToConcat = calloc(1024,sizeof(char));
	if(machine == true && user == true){
		strcpy(stringToConcat,shellName); // shell name added to empty string
		strcat(stringToConcat,"-");//sfish-
		strcat(stringToConcat,currentUserColor);//Setting the user color of the terminal(begin_fill)
		strcat(stringToConcat,getenv("USER"));//sfish-user
		strcat(stringToConcat,"\e[0m");//(end_fill)

		strcat(stringToConcat,"@");//sfish-user@

		char * hostname = calloc(256,sizeof(char));
		gethostname(hostname, 256);
		strcat(stringToConcat,currentMachineColor);//Setting the machine color (begin_fill)
		strcat(stringToConcat,hostname);//sfish-user@machine
		strcat(stringToConcat,"\e[0m");//(end_fill)

		strcat(stringToConcat,":");//sfish-user@machine:
		free(hostname);
	}
	else if(machine == false && user == true){
		strcpy(stringToConcat,shellName); // shell name added to empty string
		strcat(stringToConcat,"-");//sfish-
		strcat(stringToConcat,currentUserColor);//Setting the user color of the terminal(begin_fill)
		strcat(stringToConcat,getenv("USER"));//sfish-user
		strcat(stringToConcat,"\e[0m");//(end_fill)
		strcat(stringToConcat,":");//sfish-machine:
	}
	else if(machine == true && user == false){
		strcpy(stringToConcat,shellName); // shell name added to empty string

		strcat(stringToConcat,"-");//sfish-
		char * hostname = calloc(256,sizeof(char));
		gethostname(hostname, 256);
		strcat(stringToConcat,currentMachineColor);//Setting the machine color (begin_fill)
		strcat(stringToConcat,hostname);//sfish-machine
		strcat(stringToConcat,"\e[0m");//(end_fill)
		strcat(stringToConcat,":");//sfish-machine:
		free(hostname);
	}
	else{
		strcpy(stringToConcat,shellName); // shell name added to empty string
		strcat(stringToConcat,":");//sfish:
	}

	strcat(stringToConcat,"[");
	return stringToConcat;
}

void print_help(){
	// printf("help [-dms] [pattern ...]\n");
	// strcpy(buf,"test help\n");
	// write(STDOUT_FILENO, buf, 10);
	printf("help  - Print a list of all builtin’s and their basic usage in a single column. Type help in bash to get an idea.\n");
	printf("exit  - Exits the shell.\n");
	printf("pwd   - Changes the current working directory of the shell.\n");
	printf("prt   - Prints the return code of the command that was last executed..\n");
	printf("chpmt - Change prompt settings\n");
	printf("\tUsage: chpmt SETTING TOGGLE\n");
	printf("\tValid values of TOGGLE: 1 - Enabled, 0 - Disabled\n");
	printf("\tValid values for SETTING are:\n");
	printf("\t\tuser : The user field in the prompt.\n");
	printf("\t\tmachine : The context field in the prompt\n");
	printf("chclr - Change prompt colors\n");
	printf("\tUsage: chclr SETTING COLOR BOLD\n");
	printf("\tValid values for SETTING are:\n");
	printf("\t\tuser : The user field in the prompt.\n");
	printf("\t\tmachine : The context field in the prompt\n");
	printf("\tValid values for COLOR are:\n");
	printf("\t\tred : ANSI red.\n");
	printf("\t\tblue : ANSI blue.\n");
	printf("\t\tgreen : ANSI green.\n");
	printf("\t\tyellow : ANSI yellow.\n");
	printf("\t\tcyan : ANSI cyan.\n");
	printf("\t\tmagenta : ANSI magenta.\n");
	printf("\t\twhite : ANSI white.\n");
	printf("\t\tblack : ANSI black.\n");
	printf("\tTo toggle BOLD use:\n");
	printf("\t\t1 : Bold Enabled.\n");
	printf("\t\t0 : Bold Disabled.\n");
	printf("jobs - List all jobs running in the background, their name, PID, job numberE\n");
	printf("fg   - This builtin command should make the specified job number in your list go to the foreground\n");
	printf("\tUsage: fg PID|JID\n");
	printf("bg   - This builtin command should send a signal to the specified job in your job list to resume its execution in the background.\n");
	printf("\tUsage: bg PID|JID\n");
	printf("kill - This builtin command should send the specified signal to the process specified by PID. If no signal is specified you must send SIGTERM as default. The signal is denoted by its number ranging from 1 to 31 (inclusive).\n");
	printf("\tUsage: kill [signal] PID|JID\n");
	printf("disown - This builtin command should delete the specified job from the job list. If no PID is specified you must delete all jobs from the job list.\n");
	printf("\tUsage: disown [PID|JID]\n");
	printf("Ctrl+C : Kills foreground program(s) using SIGINT.\n");
	printf("Ctrl+Z : Suspends foreground program(s) using SIGTSTP.\n");
	printf("Ctrl+B : Store PID\n");
	printf("Ctrl+G : Get PID\n");
	printf("Ctrl+H : Print Help Menu\n");
	printf("Ctrl+P : Print SFISH Info\n");

}

void changeDirectory(char * directoryPath){
	// printf("%s %s\n", "changeDirectory: ", directoryPath);
	// printf("HOME : %s\n", getenv("HOME"));
	// strcpy(lastWorkingDirectory,getcwd(shellDirectory,1024));
	// printf("Last working directory : %s\n", lastWorkingDirectory);
	if (directoryPath == NULL){
		if(chdir(getenv("HOME"))==-1){
			// printf("%s\n", "Invalid directory");
		}
		else{
			// printf("%s\n", "valid directory");
		}
	}
	else{
		if(strcmp(directoryPath,".")==0){//current directory
			lastReturnValue = chdir(".");
		}
		else if(strcmp(directoryPath,"..")==0){//parent directory
			lastReturnValue = chdir("..");
		}
		else if(strcmp(directoryPath,"-")==0){
			// printf("%s\n", "Pass" );
			lastReturnValue = chdir(lastWorkingDirectory);
		}
		else if (directoryPath != NULL){
			lastReturnValue = chdir(directoryPath);
		}
	}
}

void getpwdHelper(char * shellDirectory){
	printf("Present working directory : %s\n",getcwd(shellDirectory,1024));
	lastReturnValue = EXIT_SUCCESS; //Setting the exit value on passing
}



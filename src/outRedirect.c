#include "sfish.h"
void fileFinder(char** newArgV){

	pipes = newArgC -1;
	// printf("newArgC : %d\n", newArgC);
	if(pipes == 0){
		IN_FD = STDIN_FILENO;
		OUT_FD = STDOUT_FILENO;
		redirectionParse(newArgV[0]);
	}else if(pipes > 0){//With pipes
		IN_FD = STDIN_FILENO;
		OUT_FD = STDOUT_FILENO;
		sigset_t mask_one, prev_one;
		sigemptyset(&mask_one);
		sigaddset(&mask_one, SIGCHLD);
		if(backgroundTask){//If background process
			pid_t pid = fork();
			if(pid==0){
				// printf("%s\n", "====CHILD====");
				setpgid(0,0);
				sigprocmask(SIG_BLOCK,&mask_one,&prev_one);
				pipingExecute(newArgV, pipes);
				sigprocmask(SIG_SETMASK,&prev_one, NULL);
				exit(0);
			}
			currentFD = pid;
			addNode(buffer,pid,pid,pipes+1,"Running");
		}else{//If foreground process
			sigprocmask(SIG_BLOCK,&mask_one,&prev_one);
			pipingExecute(newArgV, pipes);
			// signal(SIGCHLD,SIG_IGN);
			sigprocmask(SIG_SETMASK,&prev_one, NULL);		
		}

	}//ELSE HANDLES THE NULL INPUT
}


void pipingExecute(char ** arg , int pipes){//grep .c < out.txt | ls -l  > in.txt
	// int status;
	pid_t pid;
	pid_t pgid = 0;
	int pipefds[2*pipes];

	for(int i = 0; i < pipes; i++){
		if(pipe(pipefds + i*2) < 0) {
			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
	int fdCount = 0;
	pgid++;
	for(int commandCount = 0; commandCount < pipes+1; commandCount++){
		IN_FD = STDIN_FILENO;
		OUT_FD = STDOUT_FILENO;
		if(arg[commandCount]==NULL){
			break;
		}
		char ** command = calloc(1024,sizeof(char));
		char ** commandCopy = calloc(1024,sizeof(char));
		char ** builtExecutable = calloc(1024,sizeof(char));
		char ** builtExecutableCopy = calloc(1024,sizeof(char));
		command = tokenizeSpaces(arg[commandCount]);
		commandCopy = memcpy(commandCopy,command, 1024);
		builtExecutable = buildExecutable(command);
		builtExecutableCopy = memcpy(builtExecutableCopy,builtExecutable,1024);

		char * file = calloc(1024,sizeof(char));
		file = getCommand(builtExecutableCopy[0]);
		if(file == NULL){
			printf("%s: command not found\n", command[0]);
			break;
		}

		pid = fork();        //if not first command
		if(pid < 0){
			printf("%s\n", "error");
		}
		else if(pid == 0){
			// printf("%s\n", "====CHILD====");
			// pid = getpid ();
			// if (pgid == 0){ pgid = pid;}

			// setpgid (pid, pgid);
			// if (!backgroundTask){
			// 	tcsetpgrp (STDOUT_FILENO, pgid);
			// }
			// signal (SIGINT, SIG_DFL);
			// signal (SIGQUIT, SIG_DFL);
			// signal (SIGTSTP, SIG_DFL);
			// signal (SIGTTIN, SIG_DFL);
			// signal (SIGTTOU, SIG_DFL);
			// signal (SIGCHLD, SIG_DFL);

	        if(fdCount != 0){//Get the read FDs
	        	dup2(pipefds[fdCount-2], STDIN_FILENO);
	        	// if(dup2(pipefds[(fdCount-1) * 2], STDIN_FILENO) < 0){
	         //        perror(" dup2");///j-2 0 j+1 1
	         //        exit(EXIT_FAILURE);
	         //    }
	        } 
	        //if not last command
	        if(commandCount < pipes){//if(fdCount < pipes+1){//Get the write FDs
	        	dup2(pipefds[fdCount + 1], STDOUT_FILENO);
	        	// if(dup2(pipefds[fdCount * 2 + 1], STDOUT_FILENO) < 0){
	        	// 	perror("dup2");
	        	// 	exit(EXIT_FAILURE);
	        	// }
	        }
	        if(IN_FD!=STDIN_FILENO){
	        	dup2(IN_FD,STDIN_FILENO);
	        	close(IN_FD);
	        }
	        if(OUT_FD!=STDOUT_FILENO){
	        	dup2(OUT_FD,STDOUT_FILENO);
	        	close(OUT_FD);
	        }

	        for(int i = 0 ; i < 2*pipes; i++){
	        	close(pipefds[i]);
	        }
	        if(fileExists(file)==true){
	        	execvp(file,builtExecutableCopy);	
	        	exit(EXIT_FAILURE);
	        }else{
	        	exit(EXIT_FAILURE);
	        }
	    }else{
	    	// printf("%s\n", "====PARENT====");
	    	fdCount+=2;

	    }
	}
	for(int i = 0; i < 2 * pipes; i++){
		close(pipefds[i]);
	}
	for(int i = 0; i < pipes+1; i++){
		waitpid(pid,NULL,0);
	}
}


void redirectionParse(char * arg){//Seperates each of them on spaces
	char ** command = calloc(1024,sizeof(char));
	char ** builtExecutable = calloc(1024,sizeof(char));
	command = tokenizeSpaces(arg);
	builtExecutable = buildExecutable(command);
	redirectionExecute(builtExecutable);
}

char ** tokenizeSpaces(char * arg){
	char ** command = calloc(4096,sizeof(char));
	char * string = calloc(1024,sizeof(char));
	int count = 0;
	string = strcpy(string,arg);
	string = strtok(string," ");
	command[count] = string;

	while(command[count]!= NULL){//this tokenizes the string on spaces
		count++;
		command[count] = strtok(NULL," ");
	}
	return command;
}

char ** buildExecutable(char ** arg){
	char ** command = calloc(4096,sizeof(char));
	int count = 0;
	char * input = calloc(4096,sizeof(char));
	char * output = calloc(4096,sizeof(char));
	while(arg[count] != NULL){
		//This builds the array that needs to be passed into char * getCommand(char * currentCommand)
		if(strcmp(arg[count],">")==0 || strcmp(arg[count],"<")==0 || strcmp(arg[count],"2>")==0){
			break;
		}
		else{
			char * string = calloc(1024,sizeof(char));
			string = strcpy(string,arg[count]);
			command[count] = string;
		}
		count++;
	}
	command[count] = NULL;

	while(arg[count]!=NULL){//This finds the output and input file // more error handling required
		if(strcmp(arg[count],">")==0){
			count++;
			if(arg[count]==NULL){
				printf("%s\n", "Invalid arguement");
				return NULL;
			}else{
				output = strcpy(output,arg[count]);
				OUT_FD  = open(output, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			}
		}else if(strcmp(arg[count],"<")==0){
			count++;
			if(arg[count]==NULL){
				printf("%s\n", "Invalid arguement");
				return NULL;
			}else{
				input = strcpy(input,arg[count]);
				IN_FD  = open(input, O_RDONLY);
			}
		}
		else if(strcmp(arg[count],"2>")==0){
			OUT_FD  = STDERR_FILENO;
		}
		count++;
	}
	return command;
}



void redirectionExecute(char ** command){
	char * executable = calloc(1024,sizeof(char));
	executable = getCommand(command[0]);

	if(executable == NULL){
		printf("%s: command not found\n", command[0]);
		return;
	}

	pid_t pid = fork();
	if(pid == 0){
		// printf("%s\n", "====CHILD====");
		if(fileExists(executable) == true){
			dup2(IN_FD,STDIN_FILENO);
			dup2(OUT_FD,STDOUT_FILENO);
			if(IN_FD != STDIN_FILENO){
				// printf("%s\n", "IN_FD");
				close(IN_FD);
			}
			if(OUT_FD != STDOUT_FILENO){
				// printf("%s\n", "OUT_FD");
				close(OUT_FD);
			}
			execvp(executable,command);
			// exit(0);
		}
		else{
			printf("%s: command not found\n", command[0]);
			exit(EXIT_FAILURE);
		}
	}else{
		// printf("%s\n", "====PARENT====");
		if(!backgroundTask){//if foreground
			currentFD = pid;
			waitpid(pid,NULL,0);
		}
	}

	if(backgroundTask){
		addNode(buffer, pid,pid, 1, "Running");
	}

}

char * getCommand(char * currentCommand){
	char * path = calloc(1024,sizeof(char));
	char * envPATH = getenv("PATH");
	char * envPathCopy = calloc(1024,sizeof(char));
	// printf("path : %p\n", path);

	strcpy(envPathCopy,envPATH);
	path = strtok(envPathCopy,":");
	char * file = calloc(1024,sizeof(char));
	while(path!=NULL){

		strcpy(file,path);
		strcat(file,"/");
		strcat(file,currentCommand);

		if(fileExists(file) == true){
			return file;
		}
		path = strtok(NULL,":");
		// free(file);
		file = calloc(1024,sizeof(char));	
	}
	// free(envPathCopy);
	// free(path); //This is not freeing
	return NULL;
}

bool fileExists(const char* file) {
	struct stat buf;
	return (stat(file, &buf) == 0);
}
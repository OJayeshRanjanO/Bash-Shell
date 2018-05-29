#include "sfish.h"
int totalJobs = 1;
void addNode(char * jobName, int PID, int GID, int process,char * status){
	job * node = NULL;
	if(head==NULL){//Add to the head
		node = calloc(sizeof(job),sizeof(char));

		node->jobName = jobName;
		node->JID = totalJobs;
		node->PID = PID;
		node->GID = GID;
		node->process = process;
		node->status = status;
		if(backgroundTask){
			node->fg  = false;
		}

		node->next = NULL;

		head = node;
	}
	else{//Add to the tail
		job * node = head;
		node = iterator(node);

		node = calloc(sizeof(job),sizeof(char));
		node->jobName = jobName;
		node->JID = totalJobs;
		node->PID = PID;
		node->GID = GID;
		node->process = process;
		node->status = status;
		if(backgroundTask){
			node->fg  = false;
		}

		getPreviousNode(NULL)->next = node;
	}
	totalJobs++;
}

bool removeNode(int pid){
	job * node = NULL;
	node = nodeFinder(pid);
	if(node==NULL){
		return false;
	}
	else{
		if(head == node){
			job * newHead = head->next;
			if(newHead!=NULL){
				free(node);//removing the old head
				head = newHead;
			}//else do nothing. The whole list is empty
			else{
				free(node);
				head = NULL;
			}
		}
		else{
			job * parent = getPreviousNode(node);
			job * child = node->next;
			parent->next = child;
			if(node!=NULL){
				free(node);
			}
		}
	}
	totalJobs = 1;
	job * cursor = head;
	while(cursor!=NULL){
		cursor->JID = totalJobs;
		cursor = cursor->next;
		totalJobs++;
	}
	return true;
}

job * getPreviousNode(job * child){
	job * parent = head;
	while(parent->next!=child){
		parent = parent->next;
	}
	return parent;
}

void printer(job * node){
	job * cursor = node;
	// printf("%s\n", "=====PRINTER=====");
	while(cursor!=NULL){
		printf("[%d]\t%s \t%d\t%s\n", cursor->JID,cursor->status,cursor->PID,cursor->jobName);
		cursor = cursor->next;
	}
}

job * nodeFinder(pid_t pid){
	job * cursor = head;
	while(cursor!=NULL){
		if(cursor->PID == pid){
			return cursor;
		}
		cursor = cursor->next;
	}
	return NULL;
}

job * iterator(job * cursor){
	while(cursor!=NULL){
		cursor = cursor->next;
	}
	return cursor;
}

bool pidChecker(pid_t pid){
	job * cursor = head;
	while(cursor!=NULL){
		if(cursor->PID == pid){
			return true;
		}
		cursor = cursor->next;
	}
	return false;
}

void handleChild(int sig){
	pid_t pid;
	int status;
	int saved_errno = errno;
	sigset_t mask_one, prev_one;
	sigemptyset(&mask_one);
	sigaddset(&mask_one, SIGCHLD);
	while((pid = waitpid(-1,&status, WNOHANG | WUNTRACED | WCONTINUED | WSTOPPED)) > 0 ){
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			sigprocmask(SIG_BLOCK,&mask_one,&prev_one);
			removeNode(pid);
			sigprocmask(SIG_SETMASK,&prev_one, NULL);
		}else if(WIFSTOPPED(status)){
			job * node = calloc(sizeof(job),sizeof(char));
			node = nodeFinder(pid);
			node->status = "Stopped";
			//get job pid and change the state to stop
		}else if(WIFCONTINUED(status)){
			job * node = calloc(sizeof(job),sizeof(char));
			node = nodeFinder(pid);
			node->status = "Running";
			//get job pid and change it's status to running
		}
	}
	errno = saved_errno;

}

void handleInterruption(int sig){//Ctrl C


}

void switchToFG(pid_t pid){
	job * node = head;
	while(node!=NULL){
		if(node->PID == pid){
			break;
		}
		node = node->next;
	}

	waitpid(node->PID,NULL,0);

	node->fg = true;
}

void switchToBG(pid_t pid){
	job * node = head;
	while(node!=NULL){
		if(node->PID == pid){
			break;
		}
		node = node->next;
	}

	waitpid(node->PID,NULL,0);

	node->fg = false;
}

int getPID(char ** userInput){
	int numArgs = 0 ;
	while(userInput[numArgs]!=NULL){//count args
		// printf("%s\n", userInput[numArgs]);
		numArgs++;
	}
	char * arguement = calloc(1024,sizeof(char));
	strcpy(arguement,userInput[numArgs-1]);
	if(head!=NULL){
		if(arguement[0]=='%'){
			if(arguement[1]>='0' && arguement[1] <= '9'){
				job * node = head;
				while(node!=NULL){
					if(node->JID==arguement[1]-'0'){
						break;
					}
					node = node->next;
				}
				return node->PID;
			}
			else{
				return -1;//failure
			}
		}else{
			if(atoi(userInput[numArgs-1])!=-1){
				return atoi(userInput[numArgs-1]);
			}
			return -1;//failure
		}
	}
	else{
		return -1;//failure
	}
}

void printSFISH(){
	printf("----Info----\n");
	printf("help\n");
	printf("prt\n");
	printf("----CTRL----\n");
	printf("cd\n");
	printf("chclr\n");
	printf("chpmt\n");
	printf("pwd\n");
	printf("exit\n");
	printf("----Job Control----\n");
	printf("fg\n");
	printf("bg\n");
	printf("disown\n");
	printf("jobs\n");
	printf("---Number of Commands Run----\n");
	printf("%d\n", processCount);
	printf("----Process Table----\n");
	printf("PGID\tPID\tTIME\tCMD\n");
	job * node = head;
	while(node!=NULL){
		printf("%d\t%d\t%s\t%s\n", node->GID, node->PID,"00:00",node->jobName);
		node = node->next;
	}

}


#include  <stdio.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
int na=0;
char back='q';
int nb=0;//number of background processes;
pid_t cf=-1;
struct Node {
    int proc;
    struct Node* next;
};
/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
    if(i+1==strlen(line) && strcmp(token,"&")==0){
        back='&';
        continue;
    }
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
  na=tokenNo;
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}
void handler(int sig){
	setpgid(cf,0);
	killpg(cf,SIGINT);
	printf("\n");
}
void background_clean(struct Node *head){
	int fl=0,status;
	struct Node* upd = NULL;
	//struct Node* head = NULL;
    struct Node* ne = NULL;
    struct Node* cur = NULL;
	cur=(struct Node*)malloc(sizeof(struct Node));
	ne=head;
	//printf("head->data : %d\n",head->proc);
			while(ne!=NULL){
				if(waitpid(ne->proc,&status,WNOHANG) && WIFEXITED(status)){
					pid_t pp=waitpid(ne->proc,&status,WNOHANG);
					printf("background process is reaped with pid : %d\n",ne->proc);
					nb=nb-1;
				}
				else{
					if(fl==0){
						upd = (struct Node*)malloc(sizeof(struct Node));
						upd=ne;
						cur=ne;
						fl=1;
					}
					else{
						cur->next=ne;
						cur=ne;
					}
				}
				ne=ne->next;
			}
			cur->next=NULL;
			head=upd;
}
int main(int argc, char* argv[]) {
	signal(SIGINT,handler);
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,status,d;
	struct Node* head = NULL;
    struct Node* ne = NULL;
    struct Node* cur = NULL;
	while(1) {			
		/* BEGIN: TAKING INPUT */   
		if(WIFEXITED(status))
		background_clean(head);
		int status;
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		//printf("no of tokens : %d\n",na);
		if(tokens[0]==NULL)
		continue;
		int res1=strcmp(tokens[0],"cd");
		int res2=strcmp(tokens[0],"exit");
		if(res1!=0 && res2!=0){
        int rc=fork();
		if(back!='&')
		cf=rc;
		if(rc<0){
			printf("Child process is not created , it is failed!\n");
		}
		else if(rc==0){
			execvp(tokens[0],tokens);
			printf("execution failed\n");
			return 1;
		}
		else{
			if(WIFEXITED(status))
			background_clean(head);
            if(back=='&'){
				setpgid(rc,0);
                pid_t pid=waitpid(rc,&status,WNOHANG);
                //while(waitpid(rc,&status,WNOHANG)<0);
				if(pid==0){
				if(nb==0){
					head = (struct Node*)malloc(sizeof(struct Node));
					cur = (struct Node*)malloc(sizeof(struct Node));
					head->proc=rc;
					head->next=NULL;
					cur=head;
				}
				else{
					ne = (struct Node*)malloc(sizeof(struct Node));
					ne->proc=rc;
					ne->next=NULL;
					cur->next=ne;
					cur=ne;
				}
				nb++;
				}
            }
            else{
			pid_t wc = waitpid(rc,&status,0);
			//if(WIFEXITED(status))
			//background_clean(head);
			if(wc==-1){
				printf("child is not repead , it will remain as a zombie !\n");
			}
			else{
				//printf("successfull");
			}
            }
		}
		}
		else if(res1==0){
			if(na>2){
				printf("cd : too many arguments \n");
			}
			else{
			char *path=tokens[1];
			int dir=chdir(path);
			if(dir<0)
			printf("no such file or directory exist\n");
			else
			printf("directory is successfully changed\n");
			//printf("%s",tokens[1]);
			}
		}
		else{
			if(strcmp(tokens[0],"exit")==0){
				cur=head;
				while(cur!=NULL){
					int ret=kill(cur->proc,1);
					waitpid(cur->proc,&status,0);
					cur=cur->next;
				}
				free(tokens);
				free(head);
				free(cur);
				break;
			}
		}
       //do whatever you want with the commands, here we just print them

		//for(i=0;tokens[i]!=NULL;i++){
			//printf("found token %s (remove this debug output later)\n", tokens[i]);
		//}
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
        back='q';
	}
	return 0;
}

//
//  operating-system.c
//  labOS
//
//  Created by Nathalie von Huth on 25/10/2016.
//  Copyright © 2016 Nathalie von Huth. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

static char *my_argv[100], *my_envp[100];
typedef void (*sighandler_t)(int);
// char c = '\0';


void handle_signal(int signo, const char* cwd)
{
	printf("\n%s>", cwd);
	fflush(stdout);
}
/*
	This function matches strings with regular expressions and is
	used when checking that PATH and HOME are assigned to something.
*/
int match(const char *string, char *pattern){
	int status;
	regex_t re;

	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
		return(0);      /* Report error. */
	}
	status = regexec(&re, string, (size_t) 0, NULL, 0);
	regfree(&re);
	if (status != 0) {
		return(0);      /* Report error. */
	}
	return(1);
}

//Gets the current working directory
const char *getCWD(){
	static char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
		return cwd;
	} else {
		perror("getcwd() error");
		return 0;
	}
}
//Checks the contents of the .profile file
void checkProfile(const char* cwd){
	char *home = "HOME=/home/os\n";
	char *path = "PATH=/bin:/usr/bin:/usr/local/bin\n";

	//Gets current working directory and looks for the .profile file
	char str[1024];
	strcpy(str, cwd);
	strcat(str, "/.profile");
	
	FILE *ptr_file;
	char buf[1000];

	//If file exists then check if PATH and HOME are assigned properly
	if(access(str, F_OK ) != -1) {
		ptr_file =fopen(str,"r");
		char line[256];
		char *pathstring = "PATH=/(.+)";
		char *homestring = "HOME=/(.+)";

		int index = 0;
		while (fgets(line, sizeof(line), ptr_file)) {
			if(match(line, pathstring) == 1){
				my_envp[0] = strdup(line); //my_envp[0] is PATH
			} else if(match(line, homestring) == 1){
				my_envp[1] = strdup(line); //my_envp[0] is HOME
			} else {
				perror("Error: HOME or PATH variables are not assigned properly");
			}
		}
	//If the file doesn't exist print error
	} else {
		perror(".profile file doesn't exist");
	}
	
	fclose(ptr_file);
}

void fill_argv(char *tmp_argv)
{
	char *foo = tmp_argv;
	int index = 0;
	char ret[100];
	bzero(ret, 100);
	while(*foo != '\0') {
		if(index == 10)
			break;

		if(*foo == ' ') {
			if(my_argv[index] == NULL)
				my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
			else {
				bzero(my_argv[index], strlen(my_argv[index]));
			}
			strncpy(my_argv[index], ret, strlen(ret));
			strncat(my_argv[index], "\0", 1);
			bzero(ret, 100);
			index++;
		} else {
			strncat(ret, foo, 1);
		}
		foo++;
		/*printf("foo is %c\n", *foo);*/
	}
	my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
	strncpy(my_argv[index], ret, strlen(ret));
	strncat(my_argv[index], "\0", 1);
}

int attach_path(char *cmd)
{
	char ret[100];
	int index;
	int fd;
	bzero(ret, 100);
	for(index=0;my_envp[index]!=NULL;index++) {
		strcpy(ret, search_path[index]);
		strncat(ret, cmd, strlen(cmd));
		if((fd = open(ret, O_RDONLY)) > 0) {
			strncpy(cmd, ret, strlen(ret));
			close(fd);
			return 0;
		}
	}
	return 0;
}

int main(int argc, char *argv[], char *envp[])
{
	char c;
	int i, fd;
	const char* startupSign  = getCWD();
	char *tmp = (char *)malloc(sizeof(char) * 100);
	char *cmd = (char *)malloc(sizeof(char) * 100);
	
	signal(SIGINT, SIG_IGN);
	if(fork() == 0){
		execve("/usr/bin/clear", argv, my_envp);
	} else {
		wait(NULL);
	}
	//signal(SIGINT, handle_signal;

	printf("%s> ", startupSign);

	while(c != EOF) {
		c = getchar();
		switch(c){
			case '\n':
				if(tmp[0] == '\0'){
					printf("%s> ", startupSign);
				} else {
					fill_argv(tmp);
					strncpy(cmd, my_argv[0], strlen(my_argv[0]));
					strncat(cmd, "\0", 1);
					if(index(cmd, '/') == NULL) {
						if(attach_path(cmd) == 0) {
							call_execve(cmd);
						} else {
							printf("%s: command not found\n", cmd);
						}
					} else {
						if((fd = open(cmd, O_RDONLY)) > 0) {
							close(fd);
							call_execve(cmd);
						} else {
							printf("%s: command not found\n", cmd);
						}
					}
					free_argv();
					printf("[MY_SHELL ] ");
					bzero(cmd, 100);
				}
				bzero(tmp, 100);
				break;
			default: strncat(tmp, &c, 1);
				 break;
		}
		if(c == '\n')
			printf("%s>\n", startupSign);
	}
	printf("\n");
	// writeEnvVar(startupSign);
	checkProfile(startupSign);
	return 0;
}
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
#include <fcntl.h> // for open
#include "operating-system.h"

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
	}
	return 0;
}

//Gets the string of PATH
char *getPATH(FILE *ptr_file){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char *pathstring = "PATH=/(.+)";
	while ((read = getline(&line, &len, ptr_file)) != -1) {
		if(match(line, pathstring) == 1){
			char *subPath = malloc(sizeof(char) * 100);
			memcpy(subPath, &line[5], 100);
			return subPath;
		}
	}
	return 0;
}
//Gets the string of HOME
char *getHOME(FILE *ptr_file){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char *homestring = "HOME=/(.+)";
	while ((read = getline(&line, &len, ptr_file)) != -1) {
		if(match(line, homestring) == 1){
			char *subPath = malloc(sizeof(char) * 100);
			memcpy(subPath, &line[5], 100);
			return subPath;
		}
	}
	return 0;
}

//Reads the content of .profile
FILE *readProfile(){
	FILE *ptr_file;
	if(access(".profile", F_OK ) != -1) {
		ptr_file =fopen(".profile","r");
		return ptr_file;
	} else{
		perror(".profile file doesn't exist");
	}
	fclose(ptr_file);
	return 0;
}

void getArguments(char *tmp){
	char **my_argv = (char**)calloc(1024, sizeof(char*));
	char *token;
	int index = 0;

	token = strtok(tmp, " ");
	while(token != NULL){
		for(int j = 0; j < sizeof(token); j++){
			my_argv[index] = (char*)calloc(1024, sizeof(char));
			my_argv[index][j] = token[j];
			printf("%c", my_argv[index][j]);
		}
		token = strtok(NULL, " ");
		index++;
	}
}

//Checks the contents of .profile
void checkProfile(){
	FILE *ptr_file = readProfile();
	char *path = getPATH(ptr_file);
	fclose(ptr_file);
}

int main(int argc, char *argv[], char *envp[])
{
	char c;
	int i, fd;
	const char* startupSign  = getCWD();
	char *tmp = (char *)malloc(sizeof(char) * 100);
	char *cmd = (char *)malloc(sizeof(char) * 100);
	
	signal(SIGINT, SIG_IGN);
	printf("%s> ", startupSign);

	while(c != EOF) {
		c = getchar();
		tmp[i] = c;

		if(c == '\n'){
			tmp[i] = '\0';
			if(tmp[0] == '\0'){
				printf("%s>", startupSign);
			}
			else{
				getArguments(tmp);
				printf("%s>", startupSign);
				i = 0;
			}
		} else {
			i++;
		}
	}
	printf("\n");
	// writeEnvVar(startupSign);
	checkProfile();
	return 0;
}
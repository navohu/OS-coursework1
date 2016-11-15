//
//  operating-system.c
//  labOS
//
//  Created by Nathalie von Huth on 25/10/2016.
//  Copyright © 2016 Nathalie von Huth. All rights reserved.
//

#include <dirent.h>
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

char **getArguments(char *tmp){
	char **my_argv = (char**)calloc(1024, sizeof(char*));
	char *token;
	int index = 0;

	token = strtok(tmp, " ");
	while(token != NULL){
		my_argv[index] = (char*)malloc(sizeof(char) * 100);
		my_argv[index] = token;
		token = strtok(NULL, " ");
		index++;
	}
	return my_argv;
}

char **tokenizePath(char *path){
	char **tokens = (char**)malloc(sizeof(char*) * 1024);
	char *token;
	int index;
	token = strtok(path, ":");

	while(token != NULL){
		tokens[index] = (char*)malloc(sizeof(char) * 1024);
		tokens[index] = token;
		token = strtok(NULL, ":");
		index++;
	}
	return tokens;
}

void searchProgram(char **path, char **my_argv){
	DIR *open;
	struct dirent *read;
	int index = 0;
	while(path[index] != NULL){
		open = opendir(path[index]);

		if(open == NULL){
			printf("Can't find directory: %s", path[index]);
			perror("");
			break;
		}
		while(open){
			if((read = readdir(open)) != NULL){
				if (strcmp(read->d_name, my_argv[0]) == 0) {
					closedir(open);
					printf("%s is found\n", my_argv[0]);
					break;
				}
			} else {
				closedir(open);
				printf("%s is NOT found\n", my_argv[0]);
				break;
			}
		}
	}
}

//Checks the contents of .profile
char *returnPATH(){
	FILE *ptr_file = readProfile();
	char *path = getPATH(ptr_file);
	fclose(ptr_file);
	return path;
}

int main(int argc, char *argv[], char *envp[])
{
	char c;
	int i;
	const char* startupSign  = getCWD();
	char *tmp = (char *)malloc(sizeof(char) * 100);
	char **my_argv;
	
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
				my_argv = getArguments(tmp);
				char *path = returnPATH();
				char **tokenizedPath = tokenizePath(path);
				printf("%s\n", tokenizedPath[0]);
				//searchProgram(tokenizedPath, my_argv);
				printf("%s>", startupSign);
				i = 0;
			}
		} else {
			i++;
		}
	}
	printf("\n");
	// writeEnvVar(startupSign);
	
	return 0;
}
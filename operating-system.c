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

typedef void (*sighandler_t)(int);
char c = '\0';

void writeEnvVar(const char* cwd){
	FILE *ptr_file;
	char buf[1000];

	char str[1024];
	strcpy(str, cwd);
	strcat(str, "/.profile");

	ptr_file =fopen(str,"r+");
	if (!ptr_file){
		perror("Error");
		// return 1;
	}
	while (fgets(buf,1000, ptr_file)!=NULL)
		printf("%s",buf);
	printf("\n");
	fclose(ptr_file);
}

const char *getPWD(){
	static char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
		return cwd;
	} else {
		perror("getcwd() error");
		return 0;
	}
}

void createProfile(const char* cwd){
	char *home = "HOME=/home/os";
	char *path = "PATH=/bin:/usr/bin:/usr/local/bin";

	char str[1024];
	strcpy(str, cwd);
	strcat(str, "/.profile");
	
	if( access( fname, F_OK ) != -1 ) {
		
	} else {
		// file doesn't exist
	}
}

int main(int argc, char *argv[], char *envp[])
{
	signal(SIGINT, SIG_IGN);
	const char* startupSign  = getPWD();
	printf("%s\n", startupSign);
	while(c != EOF) {
		c = getchar();
		if(c == '\n')
			printf("%s\n", startupSign);
	}
	printf("\n");
	writeEnvVar(startupSign);
	return 0;
}
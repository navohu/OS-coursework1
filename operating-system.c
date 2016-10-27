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
	char *home = "HOME=/home/os\n";
	char *path = "PATH=/bin:/usr/bin:/usr/local/bin\n";

	char str[1024];
	strcpy(str, cwd);
	strcat(str, "/.profile");
	
	FILE *ptr_file;
	char buf[1000];

	if( access(str, F_OK ) != -1 ) {
		ptr_file =fopen(str,"r+");
	} else {
		ptr_file =fopen(str,"w+");
		fputs(home, ptr_file);
		fputs(path, ptr_file);
	}
	// while (fgets(buf,1000, ptr_file)!=NULL)
	// 		printf("%s",buf);
	fclose(ptr_file);
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
	// writeEnvVar(startupSign);
	createProfile(startupSign);
	return 0;
}
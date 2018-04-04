#include <stdio.h>
#include <stdlib.h>
#define bufsize 1024

long long int filesize = 10000000; // 0.01GB

void createFile(char * fname){
	FILE * f=fopen(fname,"w");
	int r;
	char c;
	char buf[bufsize];
	int writtenBytes=0;
	while(writtenBytes<filesize){
		for(int i=0;i<bufsize;i++){	
			r=rand();
			c=(char) (r%26 + 97);
			// if(i%(bufsize-1)==0){
			// 	c='\n';
			// }
			buf[i]=c;
		}	
		fprintf(f,"%s",buf);
		writtenBytes+=bufsize;
	}
	

	fclose(f);

}

int main(int argc, char const *argv[])
{
	
	createFile("thetextsmall");
	return 0;
}
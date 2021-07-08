#include<time.h>
#include<math.h>
#include<string.h>
#include<stdio.h>
#include<malloc.h>


int main(){
	char s[] = "rename dd fd";
	char p[] = "cd da aa";
	char *sp1, *sp2, *str;
	sp1 = strtok(s, " ");
	str = strtok(NULL, " ");
	printf("str = %s\n", str);
	sp2 = strtok(p, " ");
	str = strtok(NULL, " ");
	// printf("sp1 = %s\nsp2 = %s\n", sp1, sp2);
	printf("str = %s\n", str);
	return 0;
}
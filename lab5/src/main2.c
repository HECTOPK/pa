#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "math.h"
#include <omp.h>

int main(int argc, char const *argv[]){
	int k, n;
	if(argc > 1){
		printf("Num of threads: ");
		int ret = scanf("%d", &k);
		if(ret != 1){
			printf("Bad number!\n");
			return 1;
		}
		omp_set_dynamic(0);
		omp_set_num_threads(k);
		printf("Num addings: ");
		ret = scanf("%d", &n);
		if(ret != 1){
			printf("Bad number!\n");
			return 1;
		}
	}
	else{
		FILE *f = fopen("input", "r");
		int ret = fscanf(f, "%d\n%d", &k, &n);
		fclose(f);
		if(ret < 2){
			printf("file error\n");
			return 1;
		}
	}
	struct timeval start, end;
    gettimeofday(&start, NULL);
	long double result = 0.0;
	#pragma omp parallel for reduction(+:result)
	for(int i = 1; i < n; ++i){
		long double re = ((long double)pow(-1,i+1)*4.0)/((long double)i*2-1.0);
		result += re;
	}
	gettimeofday(&end, NULL);
	int time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000;
    printf("time: %d msec\n", time);
	printf("Num of threads: %d\n", k);
	printf("Num of addings: %d\n", n);
	printf("Pi: %Lf\n", result);
	return 0;
}

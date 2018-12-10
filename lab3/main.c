#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "math.h"

struct matrix{
    int cols;
    int rows;
    int *e;
};

struct vector{
    int dim;
    int *e;
};

struct sh_mem_t{
    struct matrix matrix;
    struct vector vector;
    struct vector result;
};

void calc(struct sh_mem_t* sh_mem, int proc, int proc_n){
    int i = proc_n;
    while(i < sh_mem->matrix.rows){
        for(int j = 0; j < sh_mem->matrix.cols; ++j){
            sh_mem->result.e[i] += sh_mem->matrix.e[i * sh_mem->matrix.cols + j] * sh_mem->vector.e[j];
        }
        i += proc;
    }
}

int main(int argc, char const *argv[]){
    int n, m;
    if(argc > 1){
        printf("Num of rows: ");
        int ret = scanf("%d", &m);
        if(ret != 1){
            printf("Bad number!\n");
            return 1;
        }
        printf("Num of columns: ");
        ret = scanf("%d", &n);
        if(ret != 1){
            printf("Bad number!\n");
            return 1;
        }
    }
    else{
        FILE *f = fopen("input", "r");
        int ret = fscanf(f, "%d\n%d", &m, &n);
        fclose(f);
        if(ret < 2){
            printf("file error\n");
            return 1;
        }
    }
    printf("Num of proc: ");
    int proc;
    int ret = scanf("%d", &proc);
    if(ret != 1){
        printf("Bad number!\n");
        return 1;
    }
    //memory allocation
//    printf("alloc\n");
    int sm1 = shmget(IPC_PRIVATE, sizeof(struct sh_mem_t), 0666 | IPC_CREAT | IPC_EXCL );
//    if(sm1 == -1){
//        printf("%d\n", errno);
//        printf("EACCES: %d\n", EACCES);
//        printf("EEXIST: %d\n", EEXIST);
//        printf("EINVAL: %d\n", EINVAL);
//        printf("ENFILE: %d\n", ENFILE);
//        printf("ENOENT: %d\n", ENOENT);
//        printf("ENOMEM: %d\n", ENOMEM);
//        printf("ENOSPC: %d\n", ENOSPC);
////        printf("SHMMAX: %d\n", SHMMAX);
//    }
    struct sh_mem_t *sh_mem = (struct sh_mem_t*)shmat(sm1, NULL, 0);
//    printf("alloc2.1 %p\n", sh_mem);
    srand(time(NULL));
    sh_mem->matrix.cols = n;
    sh_mem->matrix.rows = m;
//    printf("alloc2\n");
    int sm2 = shmget(IPC_PRIVATE, n * m * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
//    if(sm2 == -1){
//        printf("sm2 %d\n", errno);
//        printf("EACCES: %d\n", EACCES);
//        printf("EEXIST: %d\n", EEXIST);
//        printf("EINVAL: %d\n", EINVAL);
//        printf("ENFILE: %d\n", ENFILE);
//        printf("ENOENT: %d\n", ENOENT);
//        printf("ENOMEM: %d\n", ENOMEM);
//        printf("ENOSPC: %d\n", ENOSPC);
//        printf("EPERM: %d\n", EPERM);
//    }
//    printf("sm1:%d\nsm2:%d\n", sm1, sm2);
    sh_mem->matrix.e = (int*)shmat(sm2, NULL, 0);
//    printf("alloc2.1 %p\n", sh_mem->matrix.e);
    for(int i = 0; i < m; ++i){
        for(int j = 0; j < n; ++j){
//            printf("%d %d\n", i, j);
            sh_mem->matrix.e[n * i + j] = rand()%199 - 99;
        }
    }
    sh_mem->vector.dim = n;
//    printf("alloc3\n");
    int sm3 = shmget(IPC_PRIVATE, n*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
    sh_mem->vector.e = (int*)shmat(sm3, NULL, 0);
    for(int j = 0; j< n; ++j){
        sh_mem->vector.e[j] = rand()%199 - 99;
    }
    sh_mem->result.dim = m;
//    printf("alloc4\n");
    int sm4 = shmget(IPC_PRIVATE, m*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
    sh_mem->result.e = (int*)shmat(sm4, NULL, 0);
    //proc creation and calc
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(int i = 0; i < proc; ++i){
        printf("before fork()\n");
        int pid = fork();
        if(pid == 0){
            calc(sh_mem, proc, i);
            return 0;
        }
    }
    for(int i = 0; i < proc; ++i){
        wait(NULL);
    }
    gettimeofday(&end, NULL);
    int time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000;
    printf("matrix: %d x %d\n", m, n);
    printf("time: %d msec\n", time);
    
    shmctl(sm1, IPC_RMID, NULL);
    shmctl(sm2, IPC_RMID, NULL);
    shmctl(sm3, IPC_RMID, NULL);
    shmctl(sm4, IPC_RMID, NULL);
	return 0;
}

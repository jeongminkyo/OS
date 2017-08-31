#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

void *fibo(void *num)
{
	pthread_t tid;
	tid = pthread_self();
	int *fibo_arr;
	int size = *(int *)num;
	int i;
	fibo_arr = (int*)malloc(sizeof(int)*size);
	fibo_arr[0] = 0;
	fibo_arr[1] = 1;
	for(i=2; i<size; i++)
	{
		fibo_arr[i] = fibo_arr[i-1] + fibo_arr[i-2];
	}
	printf("Output : ");
	for(i=0; i<size; i++)
	{
		if(i==size-1)
		printf("%d\n",fibo_arr[i]);
		else
		printf("%d,",fibo_arr[i]);
	}
	printf("\n");
}

int main()
{
	pthread_t p_thread;
	int thr_id;
	int status;
	int input;
	printf("Input : ");
	scanf("%d",&input);
	thr_id = pthread_create(&p_thread,NULL,fibo, (void *)&input);
	pthread_join(p_thread,(void **)&status);
}

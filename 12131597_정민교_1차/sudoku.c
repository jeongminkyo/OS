#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define BUF_SIZE 2

int sudoku[9][9];

typedef struct data
{
	int row;
	int col;
} data;

void *checkrow(void *p)
{
	printf("check row thread start");
	int arg = (int) p;
	int check[10]={0};
	pthread_t tid;
	tid = pthread_self();
	int i,j;
	int k=0;
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			check[sudoku[i][j]]++;
		}

		for(j=1; j<=9; j++)
		{
			if(check[j] != 1)
			{
				k++;
				break;
			}
		}
		
		for(j=1; j<=9; j++)
		{
			check[j]=0;
		}
	}
	pthread_exit((void*) k);
}

void *checkcol(void *p)
{
	printf("check col thread start");
	int arg = (int) p;
	int check[10]={0};
	pthread_t tid;
	tid = pthread_self();
	int i,j;
	int k=0;
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			check[sudoku[j][i]]++;
		}
		
		for(j=1; j<=9; j++)
		{
			if(check[j] != 1)
			{
				k++;
				break;
			}
		}
		
		for(j=1; j<=9; j++)
		{
			check[j]=0;
		}
	}
	pthread_exit((void*) k);
}
		
void *checkgrid(void *p)
{
	printf("check grid thread start");
	int check[10] = {0};
	pthread_t tid;
	tid = pthread_self();
	struct data *Dt;
	int row,col;
	int i,j;
	int k=0;
	Dt = (struct data*) p;
	row = Dt->row;
	col = Dt->col;
	
	for(i=row-2; i<=row; i++)
	{
		for(j=col-2; j<=col; j++)
		{
			check[sudoku[i][j]]++;
		}
	}

	for(j=1; j<=9; j++)
	{
		if(check[j] != 1)
		{
			k++;
			break;
		}
	}
	pthread_exit((void*) k);
}


int main(int argc, char*arg[])
{
	pthread_t thread_id[11];
	int thr_id;
	int status[11];
	int i=0;
	int j,k,l;
	int m=0;
	int row,col;
	int correct = 0;
	data dt[9];
	char *file_name;
	char buf[BUF_SIZE];
	FILE *fp;
	
	if(argc !=2)
	{
		printf("Usage : %s <input_file>\n",arg[0]);
		exit(1);
	}
	
	file_name = arg[1];
	
	if((fp = fopen(file_name,"r"))==NULL)
	{
		printf("file \' %s \' open error\n",file_name);
		fclose(fp);
		exit(1);
	}
	
	i=0;
	while(fgets(buf,BUF_SIZE,fp))
	{
		if(i%2!=0)
		{
			i++;
			continue;
		}
		row = i/18;
		col = (i/2)%9;
		sudoku[row][col] = atoi(buf);
		i++;
	}
	fclose(fp);
	
	thr_id = pthread_create(&thread_id[0],NULL,checkrow, (void *)0);
	
	if(thr_id >0)
	{
		perror("thread create error : ");
		exit(0);
	}
	
	thr_id = pthread_create(&thread_id[1],NULL,checkcol, (void *)1);
	
	if(thr_id >0)
	{
		perror("thread create error : ");
		exit(0);
	}
	
	for(i = 2; i<11; i++)
	{
		if(m==3)
		{
			m=0;
		}
		if(i<=4)
		{
			dt[i-2].row=2;
			dt[i-2].col=3*m+2;
			m++;
		}	
		else if(i<=7)
		{
			dt[i-2].row=5;
			dt[i-2].col=3*m+2;
			m++;
		}	
		else if(i<=10)
		{
			dt[i-2].row=8;
			dt[i-2].col=3*m+2;
			m++;
		}
		thr_id = pthread_create(&thread_id[i],NULL,checkgrid, (void *) &dt[i-2]);
		
		if(thr_id >0)
		{
			perror("thread create error : ");
			exit(0);
		}

	}
	
	for(i=0;i<11;i++)
	{
		pthread_join(thread_id[i],(void **) &status[i]);		
		correct += status[i];
	}
	
	if(correct == 0)
	{
		printf("Valid Result ! \n");
	}
	else
	{
		printf("Invalid Result ! \n");
	}
}
		

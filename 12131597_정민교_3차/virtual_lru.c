#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char phy_memory[65536];
struct page
{
	int frame;
	int count;
};
struct TLB
{
	int page_num;
	int frame_num;
};

struct frame
{
	int page;
	int valid;
	int virtual_address;
};
struct TLB TLB_t[16]={-1,-1};
struct frame frame_table[128]={-1,0,0};
struct page page_table[256]={-1,0};

int main(int argc, char *arg[])
{
	char *file_name;
	char buffer[256];
	FILE *fp;
	FILE *wfp = fopen("physical_lru.txt","w");
	FILE *rfp = fopen("BACKING_STORE.bin","r");
	int num=0;
	int i=0;
	int j=0;
	int k=0;
	int m;
	int hit=0;
	int total = 0;
	int TLB_hit=0;
	int check=0;
	int max=-1;
	int max2=-1;
	int lock =0;
	int victimpt;
	int maxpt;
	int victim=-1;
	for(k=0; k<16; k++)
	{
		TLB_t[k].page_num =-1;
		TLB_t[k].frame_num =-1;
	}
	
	for(k=0; k<256; k++)
	{
		page_table[k].frame =-1;
		page_table[k].count =0;
	}
	
	for(k=0; k<128; k++)
	{
		frame_table[k].page=-1;
	}
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
	

	while(EOF!=fscanf(fp,"%d",&num))
	{
		total++;
		if(i >127)//swap
		{
			for(k=0; k<16;k++)
			{
				if(TLB_t[k].page_num == num/256) //no TLB fault
				{
						TLB_hit++;
						hit++;
						fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
						check=1;
						page_table[num/256].count=0;
				}
				
			}
			if(check ==0) // TLB fault
			{
				if(page_table[num/256].frame != -1) // no page fault
				{
					for(k=0; k<16; k++)
					{
						if(page_table[TLB_t[k].page_num].count > max)
						{
							max = page_table[TLB_t[k].page_num].count;
							maxpt = k;
						}
					}
					TLB_t[maxpt].page_num = num/256;
					TLB_t[maxpt].frame_num = page_table[num/256].frame;
					page_table[num/256].count=0;
					hit++;
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
				}
				else	//fault
				{
					for(k=0; k<256; k++)
					{
						if(page_table[k].count > victim)
						{
							victim = page_table[k].count;
							victimpt = k;
						}
					}
					page_table[num/256].frame = page_table[victimpt].frame;
					frame_table[page_table[num/256].frame].page = num/256;
					frame_table[page_table[num/256].frame].valid = 1;
					page_table[victimpt].frame = -1;
					page_table[victimpt].count = 0;
					page_table[num/256].count=0;
					fseek(rfp,256*page_table[num/256].frame,SEEK_SET);
					fread(buffer,256,1,rfp);
					
					for(k=0; k<256; k++)
					{
						phy_memory[256*page_table[num/256].frame+k] = buffer[k];
					}
					
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
				
					for(k=0; k<16; k++)
					{
				
						if(page_table[TLB_t[k].page_num].count > max)
						{
							max = page_table[TLB_t[k].page_num].count;
							maxpt = k;
						}
					}
					TLB_t[maxpt].page_num = num/256;
					TLB_t[maxpt].frame_num = page_table[num/256].frame;
				}
			}
			check=0;
			max =-1;
			victim = -1;
			for(k=0; k<256; k++)
				{
					if(page_table[k].frame !=-1)
						page_table[k].count++;
				}
		
		}
		else
		{
			if(i==16)
				lock=1;
			if(i<16)
			{
				for(k=0; k<i; k++)
				{
					if(TLB_t[k].page_num == num/256)
					{
						TLB_hit++;
						hit++;
						check=1;
						page_table[num/256].count=0;
					}
				}
				if(check==0)
				{
					TLB_t[i].page_num = num/256;
					TLB_t[i].frame_num = i;
				}
			
				if(page_table[num/256].frame == -1) // page_table fault
				{
					page_table[num/256].frame = i;
					frame_table[i].page=num/256;
					frame_table[page_table[num/256].frame].valid = 1;
					page_table[num/256].count=0;
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
					fseek(rfp,256*page_table[num/256].frame,SEEK_SET);
					fread(buffer,256,1,rfp);
					for(k=0; k<256; k++)
					{
						phy_memory[256*page_table[num/256].frame+k] = buffer[k];
					}
					i++;
				}
				else
				{
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
					hit++;
					page_table[num/256].count=0;
				}
				check=0;
			}
			else
			{
				for(k=0; k<16; k++)
				{
					if(TLB_t[k].page_num == num/256) //no TLB fault
					{
						
							TLB_hit++;
							hit++;
							fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
							check = 1;
							page_table[num/256].count=0;
					}
				}
			}
			if(check==0 && i >=16 && lock==1)	//TLB fault
			{
				//tlb change
				
				if(page_table[num/256].frame == -1) // page_table fault
				{
					page_table[num/256].frame = i;
					frame_table[i].page=num/256;
					frame_table[page_table[num/256].frame].valid = 1;
					page_table[num/256].count=0;
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
					fseek(rfp,256*page_table[num/256].frame,SEEK_SET);
					fread(buffer,256,1,rfp);
					for(k=0; k<256; k++)
					{
						phy_memory[256*page_table[num/256].frame+k] = buffer[k];
					}
					
					for(k=0; k<16; k++)
					{
						if(page_table[TLB_t[k].page_num].count > max)
						{
							max = page_table[TLB_t[k].page_num].count;
							maxpt = k;
						}
					}
			
					TLB_t[maxpt].page_num = num/256;
					TLB_t[maxpt].frame_num = page_table[num/256].frame;
					i++;		
				}
				else//no fault 
				{
					for(k=0; k<16; k++)
					{
						if(page_table[TLB_t[k].page_num].count > max)
						{
							max = page_table[TLB_t[k].page_num].count;
							maxpt = k;
						}
					}
			
					TLB_t[maxpt].page_num = num/256;
					TLB_t[maxpt].frame_num = page_table[num/256].frame;
					fprintf(wfp,"Virtual address: %d Physical address: %d\n",num,256*page_table[num/256].frame + num%256);
					page_table[num/256].count=0;
					hit++;
				}
			}
			check=0;
			max = -1;
			for(k=0; k<256; k++)
			{
				if(page_table[k].frame !=-1)
					page_table[k].count++;
			}
				
		}
	}
	fclose(wfp);
	fclose(fp);
	
	FILE *wfp2 = fopen("frame_table_LRU.txt","w");
	for(i=0; i<128; i++)
	{
		fprintf(wfp2,"%d %d %d\n",i,1,frame_table[i].page * 256);
	}
	fclose(wfp2);
	
	FILE *wfp3 = fopen("physical_memory_LRU.txt","w");
	for(i=0; i<65536; i++)
	{
		fprintf(wfp2,"%d\n",phy_memory[i]);
	}
	fclose(wfp2);
	printf("LRU hit ratio : total (%d) hit (%d)\n",total,hit);
	printf("TLB hit ratio : total (%d) hit (%d)\n",total,TLB_hit);
}

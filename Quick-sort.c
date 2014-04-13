/*
 * quick-sort
 *
 * xusongqi
 *
 *2013 3 9
 * */
#include <stdio.h>  
#include <stdlib.h>
#include <time.h>
  
#define len 20000 
  
  
int Partition( int d[],int low,int high)  
{    
   int t;  
   t = d[low];  
   while(low<high)  
   {  
      while(low<high && d[high]>=t )  --high;  
      d[low] = d[high];  
      while( low<high && d[low]<=t )  ++low;  
      d[high] = d[low];  
    }  
     
   d[low] = t;  
   return low;  
   // for(int k=0;k<len;k++)  
    // printf("%d\n",d[k]);  
}  
  
void QSort(int *d,int low,int high)  
{ //对顺序表中的子序列作快速排序  
    int pivotloc;  
    if( low <high )  
    {  
        pivotloc = Partition(d,low,high);  
        QSort(d,low,pivotloc-1);  
        QSort(d,pivotloc+1,high);  
    }  
}  
  
void QuickSort(int *d)  
{  
  QSort(d,0,len-1);  
}  
  
  
  
  
  
int main()  
{    
    int d[len];  
	srand((unsigned)time(NULL));

    for(int i=0; i<len; i++)
		d[i]=rand()%20000;

    QuickSort(d);  
    for(int k=0;k<len;k++)  
     printf("%d\n",d[k]);  
    return 0;  
} 

#include <stdio.h>
#include <assert.h>
#include <algorithm>

bool g_bDebug = true;

void print(int data[], int n)
{
	for( int i=0; i<n; i++ )
		printf("%d ", data[i]);
	printf("\n");
}

// http://en.wikipedia.org/wiki/Quicksort
// http://upload.wikimedia.org/wikipedia/commons/thumb/a/af/Quicksort-diagram.svg/200px-Quicksort-diagram.svg.png
int partition(int data[], int n)
{
	int pivIdx = n-1;
	int pivot = data[pivIdx];
	
	for( int i=0; i<n-1 && pivIdx>i;  )
	{
		if(g_bDebug)
			printf("compare %d and %d\n", pivot, data[i] );

		if( pivot < data[i] )
		{
			data[pivIdx] = data[i];
			pivIdx--;
			data[i] = data[pivIdx];
			data[pivIdx] = pivot;
		}
		else
			i++;

		if(g_bDebug)
			print(data,n);
	}
	
	if(g_bDebug)
		printf( "return pivIdx %d\n", pivIdx);
	
	return pivIdx;
}

   // left is the index of the leftmost element of the array
   // right is the index of the rightmost element of the array (inclusive)
   //   number of elements in subarray = right-left+1
int partition2(int array[],  int n)
{
	int left = 0;
	int right = n-1;;
	int pivotIndex = right;

	int pivotValue = array[pivotIndex];
	std::swap( array[pivotIndex], array[right]);  // Move pivot to end
    int storeIndex = left;

    for(int  i=left; i<right; i++)  // left = i < right
	{  
		printf("compare %d and %d\n", pivotValue, array[i] );

		if (array[i] < pivotValue)
		{
			printf("sawp %d and %d\n", array[i], array[storeIndex] );

			std::swap ( array[i], array[storeIndex] );
            storeIndex++;
		}

		print(array,n);
	}
	std::swap( array[storeIndex], array[right]);  // Move pivot to its final place
    return storeIndex;
}

void myqsort(int data[], int n)
{
	int pivIdx = partition(data, n);
	//getchar();
	if(g_bDebug)
		printf("=====\n");

	if(pivIdx>1)
		myqsort(data, pivIdx);

	if(n-pivIdx-1>1)
		myqsort(data+pivIdx+1, n-pivIdx-1);
}

void test()
{
	g_bDebug = false;

#define N 1000
	int data[N];
	int data2[N];
	int n = rand() % N;

	for( int i=0; i<n; i++ )
		data[i] = data2[i] = rand();

	myqsort(data,n);
	std::sort( data2, data2+n );

	for( int i=0; i<n; i++ )
	{
		assert( data[i] == data2[i] );
	}

	printf("test %3d datas ok\n",n);
}

void test1()
{
	g_bDebug = true;

	int data[] = {3,7,8,5,2,1,9,5,4};
	//int data[] = {1,2,3,4,5,6,7,8,9};
	//int data[] = {9,8,7,6,5,4,3,2,1};
	//int data[] = {9};
	int n = sizeof(data)/sizeof(data[0]);
	//partition(data,n);
	myqsort(data,n);

	print(data,n);
}

void test2()
{
	g_bDebug = true;

	int data[] = {3,7,8,5,2,1,9,5,4};
	//int data[] = {1,2,3,4,5,6,7,8,9};
	//int data[] = {9,8,7,6,5,4,3,2,1};
	//int data[] = {9};
	int n = sizeof(data)/sizeof(data[0]);
	partition2(data,n);
	//myqsort(data,n);

	print(data,n);
}

int main()
{
	/*for( int i=0; i<1000; i++ )
		test();*/
	test2();
}
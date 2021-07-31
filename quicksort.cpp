#include <iostream>
#include <string>
#include <cstring>
#include "quicksort.h"
#include <vector>
#include <queue>
#include <numeric>
#include <bits/stdc++.h>

using namespace std;  
  

void swap(char **a,char **b)
{
    char* temp = *a;
    *a = *b;
    *b = temp;
}

//partition_func function
long long partition_func(char** arr,long low, long high)
{
    char* pivot = arr[high];
    long long i = low-1;

    //In a loop, compare every element with pivot 
    //and if the current element is small, oprswap it with  
    //element at index i
    string ps = string(pivot);
    for (long long j = low; j < high; j++)
    {
        string js = string(arr[j]);
        
        if(js.compare(0,10,ps) < 0)
        {
            i++;
            swap(&arr[i],&arr[j]);
        }
    }
    swap(arr[i+1],arr[high]);
    return i+1;
}   

//sorting function
void QS_opr(char** arr,long long low,long long high)
{
    if(low<high)
    {
        long long piv = partition_func(arr,low,high);
        QS_opr(arr,low,piv-1);
        QS_opr(arr,piv+1,high);
    }
}

void QuickSort(char** arr,long long low,long long high)
{
    QS_opr(arr,low,high);
}


void printArray(string arr[], long long size)  
{  
    int i;  
    for (i = 0; i < size; i++)  
        cout << arr[i] << " ";  
    cout << endl;  
}
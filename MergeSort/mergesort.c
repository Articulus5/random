#include <stdio.h>
#include <stdlib.h>

void merge(int *array, unsigned int first, unsigned int mid, unsigned int last)
{
     int i, j, k;

     int size1 = mid - first + 1;
     int size2 = last - mid;

     int A[size1], B[size2];

     for (i = 0; i < size1; i++)
          A[i] = array[first + i];
     for (j = 0; j < size2; j++)
          B[j] = array[mid + 1 + j];

     i = 0;
     j = 0;
     k = first;

     while (i < size1 && j < size2)
     {
          if (A[i] <= B[j])
          {
               array[k] = A[i];
               i++;
          }
          else if (B[j] < A[i])
          {
               array[k] = B[j];
               j++;
          }
          k++;
     }

     while (i < size1)
     {
          array[k] = A[i];
          i++;
          k++;
     }

     while (j < size2)
     {
          array[k] = B[j];
          j++;
          k++;
     }
}

void mergesort(int *array, unsigned int first, unsigned int last)
{
     if (first < last)
     {
          unsigned int mid = first + ((last - first) / 2);

          printf("first=%d mid=%d last=%d\n", first, mid, last);

          mergesort(array, first, mid);
          mergesort(array, mid + 1, last);

          merge(array, first, mid, last);
     }
}

void printarray(int *input, unsigned int size)
{
     if (size > 0)
     {
          printf("%d", input[0]);

          if (size > 1)
               for (int i = 1; i < size; i++)
                    printf(" %d", input[i]);

          printf("\n");
     }
}

int main(void)
{
#define SIZE 9
     int test[SIZE] = {1, 8, 2, 5, 7, 4, 6, 3, 9};

     printarray(test, SIZE);

     mergesort(test, 0, SIZE - 1);

     printarray(test, SIZE);

     return 0;
}

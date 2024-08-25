#include <cassert>
#include <climits>
#include <cstring>
#include <cstdio>

#include "ringbuf.hpp"

#define BUFFER_SIZE USHRT_MAX

int main(void)
{
     unsigned char TestBuffer[BUFFER_SIZE] = "The quick brown fox jumps over the lazy dog. ";
     unsigned char TestBuf2[256];
     unsigned char TestBuf3[256];
     int i = strlen((char *)TestBuffer);

     while (i + 1 < BUFFER_SIZE)
     {
          if (2 * i < BUFFER_SIZE)
               memcpy(&TestBuffer[i], &TestBuffer[0], strlen((char *)TestBuffer));
          else
               memcpy(&TestBuffer[i], &TestBuffer[0], BUFFER_SIZE - i - 1);
          i = strlen((char *)TestBuffer);
          printf("Filled %d/%u bytes.\n", i + 1, BUFFER_SIZE);
     }

     for (i = 0; i < 256; i++)
     {
          TestBuf2[i] = i;
     }

     {
          RingBuffer Test2(USHRT_MAX);
     }

     {
          RingBuffer Test1;

          // Fill buffer in 2 writes
          printf("Putting %u bytes, stored %u bytes.\n", 128, Test1.Put(TestBuffer, 128));
          printf("Putting %u bytes, stored %u bytes.\n", 256, Test1.Put(TestBuffer, 256));

          // Empty buffer in 2 reads
          printf("Asking for %u bytes, got %u bytes.\n", 128, Test1.Get(TestBuffer, 128));
          printf("Asking for %u bytes, got %u bytes.\n", 255, Test1.Get(TestBuffer, 255));

          // Fill buffer partway
          printf("Putting %u bytes, stored %u bytes.\n", 128, Test1.Put(TestBuffer, 128));
          
          // Get some data
          printf("Asking for %u bytes, got %u bytes.\n", 64, Test1.Get(TestBuffer, 64));

          // Fill buffer rest of the way
          printf("Putting %u bytes, stored %u bytes.\n", 255, Test1.Put(TestBuffer, 255));
          
          // Empty buffer
          printf("Asking for %u bytes, got %u bytes.\n", 512, Test1.Get(TestBuffer, 512));

          // Fill buffer partway
          printf("Putting %u bytes, stored %u bytes.\n", 192, Test1.Put(TestBuffer, 192));

          // Fill rest of buffer
          printf("Putting %u bytes, stored %u bytes.\n", 32, Test1.Put(&TestBuf2[0], 32));

          // Get some data
          printf("Asking for %u bytes, got %u bytes.\n", 192, Test1.Get(TestBuffer, 192));

          // Fill rest of the buffer
          printf("Putting %u bytes, stored %u bytes.\n", 255-32, Test1.Put(&TestBuf2[32], 255-32));

          // Get all the data
          printf("Asking for %u bytes, got %u bytes.\n", 255, Test1.Get(TestBuf3, 255));

          // Check data
          for (i = 0; i < 254; i++)
          {
               if (i != TestBuf3[i])
                    printf("Expected %d, got %u.\n", i, TestBuf3[i]);
          }
     }

     return 0;
}

#include <cstdlib>
#include <cstring>

#include "ringbuf.hpp"

#define DEF_BUF_SIZE 255

RingBuffer::RingBuffer(void)
{
     // Initialize indexes and buffer
     first = 0;
     last = 0;
     size = DEF_BUF_SIZE;
     buffer = (unsigned char *)malloc(size);
}

RingBuffer::RingBuffer(unsigned int buf_size)
{
     // Initialize indexes and buffer
     first = 0;
     last = 0;
     size = buf_size;
     buffer = (unsigned char *)malloc(size);
}

RingBuffer::~RingBuffer(void)
{
     // Free allocated buffer
     if (buffer != NULL)
          free(buffer);
}

// Returns number of bytes stored
unsigned int RingBuffer::Put(unsigned char *data, unsigned int data_size)
{
     unsigned int avail_space = size - Usage() - 1;
     unsigned int bytes_to_store = data_size;
     unsigned int bytes_stored = 0;

     // Abort if the buffer is full, and truncate data if not enough space
     if (IsFull()) return 0;
     else if (data_size > avail_space)
          bytes_to_store = avail_space;

     if (IsEmpty())
     {
          // Store data
          memcpy(&buffer[first], data, bytes_to_store);

          // Update indexes
          last = bytes_to_store;
          bytes_stored = bytes_to_store;
          bytes_to_store = 0;
     }
     else if (last < first)
     {
          // Store data
          memcpy(&buffer[last], data, bytes_to_store);

          // Update indexes
          last += bytes_to_store;
          bytes_stored += bytes_to_store;
          bytes_to_store = 0;
     }
     else // if (first < last)
     {
          if (size - last > bytes_to_store)
          {
               // Store data
               memcpy(&buffer[last], data, bytes_to_store);

               // Update indexes
               last += bytes_to_store;
               bytes_stored = bytes_to_store;
               bytes_to_store = 0;
          }
          else
          {
               // Store data
               memcpy(&buffer[last], data, size - last);

               // Update indexes
               bytes_stored += size - last;
               bytes_to_store -= size - last;
               last = 0;

               // Store data
               memcpy(&buffer[last], &data[bytes_stored], bytes_to_store);

               // Update indexes
               bytes_stored += bytes_to_store;
               last = bytes_to_store;
               bytes_to_store = 0;
          }
     }

     return bytes_stored;
}

// Returns number of bytes read
unsigned int RingBuffer::Get(unsigned char *data, unsigned int data_size)
{
     unsigned int used_space = Usage();
     unsigned int bytes_to_read = data_size;
     unsigned int bytes_read = 0;

     // Abort if the buffer is empty, and truncate read if not enough data
     if (IsEmpty()) return 0;
     else if (data_size > used_space)
          bytes_to_read = used_space;

     if (last < first)
     {
          if (size - first > bytes_to_read)
          {
               // Copy data
               memcpy(data, &buffer[first], bytes_to_read);
               memset(&buffer[first], 0, bytes_to_read);

               // Update indexes
               first += bytes_to_read;
               bytes_read += bytes_to_read;
               bytes_to_read = 0;
          }
          else
          {
               // Copy data
               memcpy(data, &buffer[first], size - first);
               memset(&buffer[first], 0, size - first);

               // Update indexes
               bytes_read += size - first;
               bytes_to_read -= size - first;
               first = 0;

               // Copy data
               memcpy(&data[bytes_read], &buffer[first], bytes_to_read);
               memset(&buffer[first], 0, bytes_to_read);

               // Update indexes
               first += bytes_to_read;
               bytes_read += bytes_to_read;
               bytes_to_read = 0;
          }
     }
     else if (first < last)
     {
          // Copy data
          memcpy(data, &buffer[first], bytes_to_read);
          memset(&buffer[first], 0, bytes_to_read);

          // Update indexes
          first += bytes_to_read;
          bytes_read += bytes_to_read;
          bytes_to_read = 0;
     }

     IsEmpty();

     return bytes_read;
}

bool RingBuffer::IsEmpty(void)
{
     if (first == last)
     {
          // Reset indexes if buffer is empty
          first = 0;
          last = 0;
          return true;
     }
     else return false;
}

bool RingBuffer::IsFull(void)
{
     if ((first == 0 && last == size - 1) || (last == first - 1))
          return true;
     else return false;
}

// Returns number of bytes used in the buffer
// If -1 is returned, something went horribly wrong
unsigned int RingBuffer::Usage(void)
{
     if (first < last)
          return last - first;
     else if (first > last)
          return (size - first + last);
     else return IsEmpty() ? 0 : -1;
}

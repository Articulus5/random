#include <inttypes.h>
#include <stdio.h>

// Returns 0 for big endian, 1 for little endian.
int test_endianess(void)
{
     volatile uint32_t i=0x01234567;
     return (*((uint8_t *)(&i))) == 0x67; // Check if first byte is 0x67
}

// value = Pointer to value casted to unsigned char pointer so we can do each byte individually
// size = Byte size of value
// spaced = Insert spaces between each byte?
// endian = Reverse byte order for little endian?
int print_bits(unsigned char *value, unsigned int size, unsigned char spaced, unsigned char endian)
{
     int chars_printed = 0;
     int i, j;

     // If little endian, start at the end and go backwards
     if (endian)
          for (i = 0; i < size - 1; i++)
               value++;

     for (i = 0; i < size; i++)
     {
          if (spaced && i) printf(" ");

          // Print bits big to small
          for (j = 7; j >= 0; j--)
               printf("%c", (*value >> j) & 1U ? '1' : '0');

          // If little endian, go backwards
          if (!endian) value++;
          else value--;
     }

     return chars_printed;
}

// value = Pointer to value casted to unsigned char pointer so we can handle variable sized values
// size = Byte size of value
// bit_value = Set bit to 0 or 1
int set_bit(unsigned char *value, unsigned int size, unsigned char bit, unsigned char bit_value)
{
     //unsigned char endian = test_endianess();
     unsigned char new_bit = bit_value ? 1 : 0;

     // Make sure the bit we want to change isn't larger than the type size
     switch(size)
     {
          case sizeof(int8_t):
               if (bit >= 8) return -1;
               *((uint8_t *)value) = (*((uint8_t *)value) & ~(1UL << bit)) | (new_bit << bit);
               break;
          case sizeof(int16_t):
               if (bit >= 16) return -1;
               *((uint16_t *)value) = (*((uint16_t *)value) & ~(1UL << bit)) | (new_bit << bit);
               break;
          case sizeof(int32_t):
               if (bit >= 32) return -1;
               *((uint32_t *)value) = (*((uint32_t *)value) & ~(1UL << bit)) | (new_bit << bit);
               break;
          case sizeof(int64_t):
               if (bit >= 64) return -1;
               *((uint64_t *)value) = (*((uint64_t *)value) & ~(1UL << bit)) | (new_bit << bit);
               break;
          default:
               return -1;
     }

     return 0;
}

// Reverse the bits in a byte in-place
unsigned int reverse_bits(unsigned int byte, unsigned int size)
{
     size *= 8;

     for (unsigned char i = 0, j = size - 1; i < j; i++, j--)
          if (((byte >> i) & 1U) != ((byte >> j) & 1U))
               byte = (byte >> i & 1U) ? (byte | (1UL << j)) & ~(1UL << i) : (byte | (1UL << i)) & ~(1UL << j);

     return byte;
}

int main(void)
{
     int32_t  tv_int    = 0xAA55F001;
     uint32_t tv_uint   = 0xF5AA0F50;
     int16_t  tv_short  = 0x55AA;
     uint16_t tv_ushort = 0xAA55;
     int8_t   tv_char   = 0x55;
     uint8_t  tv_uchar  = 0xCA;

     printf("Testing bit printing...\n");

     if (test_endianess())
          printf("System is little endian.\n");
     else printf("System is big endian.\n");

     printf("0x%0X = %d = 0b", tv_int, tv_int);
     print_bits((unsigned char *)&tv_int, sizeof(int32_t), 1, test_endianess());
     printf("\n");

     printf("0x%0X = %u = 0b", tv_uint, tv_uint);
     print_bits((unsigned char *)&tv_uint, sizeof(uint32_t), 1, test_endianess());
     printf("\n");

     printf("0x%0X = %d = 0b", tv_short, tv_short);
     print_bits((unsigned char *)&tv_short, sizeof(int16_t), 1, test_endianess());
     printf("\n");

     printf("0x%0X = %u = 0b", tv_ushort, tv_ushort);
     print_bits((unsigned char *)&tv_ushort, sizeof(uint16_t), 1, test_endianess());
     printf("\n");

     printf("0x%0X = %d = 0b", tv_char, tv_char);
     print_bits((unsigned char *)&tv_char, sizeof(int8_t), 1, test_endianess());
     printf("\n");

     printf("0x%0X = %u = 0b", tv_uchar, tv_uchar);
     print_bits((unsigned char *)&tv_uchar, sizeof(uint8_t), 1, test_endianess());
     printf("\n");

     printf("Testing bit manipulation...\n");

     // Bit operations
     unsigned int a = 60;	/* 60 = 0011 1100 */  
     unsigned int b = 13;	/* 13 = 0000 1101 */
     int c = 0;           

     // AND (if both 1)
     c = a & b;       /* 12 = 0000 1100 */ 
     printf("Line 1 - Value of c is %d\n", c );

     // OR (if one is 1)
     c = a | b;       /* 61 = 0011 1101 */
     printf("Line 2 - Value of c is %d\n", c );

     // XOR (if one is 1 but not both the same)
     c = a ^ b;       /* 49 = 0011 0001 */
     printf("Line 3 - Value of c is %d\n", c );

     // Complement (flip bits)
     c = ~a;          /*-61 = 1100 0011 */
     printf("Line 4 - Value of c is %d\n", c );

     // Shift left (make bigger)
     c = a << 2;     /* 240 = 1111 0000 */
     printf("Line 5 - Value of c is %d\n", c );

     // Shift right (make smaller)
     c = a >> 2;     /* 15 = 0000 1111 */
     printf("Line 6 - Value of c is %d\n", c );

     // Practical bit operations
     unsigned int number = 0;
     unsigned int n = 1;
     unsigned int bit = 0;

     // Set bit
     // Shift bit n times to position, then OR with number
     number |= 1UL << n;

     // Clear bit
     // Shift bit n times to position, invert it, then AND with number
     // ~(1 << 4) ~(0001 0000) -> (1110 1111)
     number &= ~(1UL << n);

     // Toggle bit
     // Shift bit n times to position, then XOR it with number
     number ^= 1UL << n;

     // Check bit
     // Shift number n times so bit to check is in 0 position, then AND it with 1
     bit = (number >> n) & 1U;

     // Testing set_bit function
     printf("Setting bit %u of %d (0x%0X, 0b", 8, tv_short, tv_short);
     print_bits((unsigned char *)&tv_short, sizeof(int16_t), 1, test_endianess());
     printf(") to %u\n", 0);
     set_bit((unsigned char *)&tv_short, sizeof(int16_t), 8, 0);
     printf("New value is %d (0x%0X, 0b", tv_short, tv_short);
     print_bits((unsigned char *)&tv_short, sizeof(int16_t), 1, test_endianess());
     printf(")\n");

     printf("Setting bit %u of %d (0x%0X, 0b", 15, tv_short, tv_short);
     print_bits((unsigned char *)&tv_short, sizeof(int16_t), 1, test_endianess());
     printf(") to %u\n", 1);
     set_bit((unsigned char *)&tv_short, sizeof(int16_t), 15, 1);
     printf("New value is %d (0x%0X, 0b", tv_short, tv_short);
     print_bits((unsigned char *)&tv_short, sizeof(int16_t), 1, test_endianess());
     printf(")\n");

     printf("Reversing bits in %u (0x%0X, 0b", tv_uchar, tv_uchar);
     print_bits((unsigned char *)&tv_uchar, sizeof(int8_t), 1, test_endianess());
     printf(")\n");
     tv_uchar = reverse_bits(tv_uchar, sizeof(uint8_t));
     printf("New value is %u (0x%0X, 0b", tv_uchar, tv_uchar);
     print_bits((unsigned char *)&tv_uchar, sizeof(int8_t), 1, test_endianess());
     printf(")\n");

     printf("Reversing bits in %u (0x%0X, 0b", tv_uint, tv_uint);
     print_bits((unsigned char *)&tv_uint, sizeof(int32_t), 1, test_endianess());
     printf(")\n");
     tv_uint = reverse_bits(tv_uint, sizeof(uint32_t));
     printf("New value is %u (0x%0X, 0b", tv_uint, tv_uint);
     print_bits((unsigned char *)&tv_uint, sizeof(int32_t), 1, test_endianess());
     printf(")\n");

     return 0;
}
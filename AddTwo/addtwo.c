#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int bit_add(int a, int b)
{
     if (!b)
          return a;

     int sum = a ^ b;            // XOR (add 0 and 1 together)
     int carry = (a & b) << 1;   // AND (track 1 and 1 then shift carry)

     return bit_add(sum, carry); // Repeat/recurse until carry is zero
}

// Fails if a or b is too big because the intermediate calculations exceed the variable type size
int log_add(int a, int b)
{
     return log(exp(a) * exp(b)); // Fails for large numbers because math and variable type size
}

int struct_add(int a, int b)
{
     struct add {
          uint8_t x[a];
          uint8_t y[b];
     } sum;

     return sizeof(sum);
}

int pointer_add(int a, int b)
{
     uint8_t *x = NULL;  // Zero
     uint8_t *y = &x[a]; // Plus A
     uint8_t *z = &y[b]; // Plus B

     return (long int)z;
}

// Only works for positive a and b because we can't unprint a character (\b doesn't count)
unsigned int printf_add(unsigned int a, unsigned int b)
{
     int sum = printf("%*s%*s", a, "", b, ""); // Fails if a or b is negative

     printf("\n");

     return sum;
}

int asm_add(int a, int b)
{
     int c;

     // Generic assembly (compiles for x86 and arm)
     asm ("mov %1, %0;"    // Move a to c
          "add %2, %0"     // Add b to c
        : "=r" (c)         // Output
        : "r" (a), "r" (b) // Inputs
     );

     return c;
}

// Only works for 0 <= a plus b < 256
int code_add(int a, int b)
{
     int sum;
     char command[90]; // Using INT_MIN for a and b results in an 86 character string

     snprintf(command, 89, "echo \"int main(void) { return %d \x2b %d; }\" | cc \x2dxc \x2dotemp \x2d", a, b); // Compile code from STDIN
     system(command);             // Compile
     sum = system("./temp") >> 8; // Return status is shifted by 1 byte for some reason and limited to uint8_t (return value % 256)
     system("rm ./temp");         // Clean up

     return sum;
}

// Very slow if b is negative
int increment_add(int a, int b)
{
     int i;

     while (b)
     {
          // Increment a by 1
          a = ~1/2 * ~a;   // minus 1 times two's complement of a results in a incrementing by 1

          // Decrement b by 1, very slow for negative numbers because it has to loop around the wrong way
          i = 1;
          while (!(b & i)) // flip bits right to left until we find the rightmost 1 bit (ripple carry subtract)
          {                // (ex. b = 0b1000 becomes 0b1111 here while i ends up being 0b1000)
               b = b ^ i;
               i <<= 1;
          }
          b = b ^ i;       // flip the rightmost 1 bit, resulting in b being decremented by 1
     }                     // (ex. 0b1111 from above finally becomes 0b0111 [one less than 0b1000])

     return a;
}

int verify(int a, int b, int c, char *name, unsigned int line)
{
     printf("%u | %s: x = %d, y = %d, x \x2b y = %d\n", line, name, a, b, c);
     return (a + b == c) ? 0 : 1; // This file contains no plus or minus signs except for the one plus sign here for testing
}

int main(int argc, char **argv)
{
     int x, y, z = 0;

     if (argc > 2)
     {
          x = atoi(argv[1]);
          y = atoi(argv[2]);
     }
     else
     {
          x = 3;
          y = 4;
     }          
     printf("Using x=%d, y=%d\n", x, y);

     z |= verify(x, y, printf_add(x, y), "printf_add", __COUNTER__);
     z |= verify(x, y, bit_add(x, y), "bit_add", __COUNTER__);
     z |= verify(x, y, log_add(x, y), "log_add", __COUNTER__);
     z |= verify(x, y, struct_add(x, y), "struct_add", __COUNTER__);
     z |= verify(x, y, pointer_add(x, y), "pointer_add", __COUNTER__);
     z |= verify(x, y, asm_add(x, y), "asm_add", __COUNTER__);
     z |= verify(x, y, code_add(x, y), "code_add", __COUNTER__);
     z |= verify(x, y, increment_add(x, y), "increment_add", __COUNTER__);

     if (z) printf("Test failed.\n");

     return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined __LINUX__
  #include <malloc.h>
#endif

size_t my_strlen(const char *str)
{
  const char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, himagic, lomagic;

  printf("longword size: %lu\r\n", sizeof(longword));

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = str; ((unsigned long int) char_ptr
			& (sizeof (longword) - 1)) != 0;
       ++char_ptr) 
  {
    printf("The address of the char_ptr: %p\r\n", char_ptr);
    printf("The value of the char_ptr points to: %c\r\n", *char_ptr);
    printf("The result of for condition: %lu\r\n", (unsigned long int) char_ptr
			                                             & (sizeof (longword) - 1));
    printf("Loop count: %ld\r\n", char_ptr - str);
    if (*char_ptr == '\0')
      return char_ptr - str;
  }

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  printf("Executes 8-byte check!\r\n");

  longword_ptr = (unsigned long int *) char_ptr;

  printf("longword_ptr address: %p\r\n", longword_ptr);

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  himagic = 0x80808080L;
  lomagic = 0x01010101L;
  if (sizeof (longword) > 4)
    {
      /* 64-bit version of the magic.  */
      /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
      himagic = ((himagic << 16) << 16) | himagic;
      lomagic = ((lomagic << 16) << 16) | lomagic;
    }
  if (sizeof (longword) > 8)
    abort ();

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      longword = *longword_ptr++;

      if (((longword - lomagic) & ~longword & himagic) != 0)
      {
        /* Which of the bytes was the zero?  If none of them were, it was
          a misfire; continue the search.  */

        const char *cp = (const char *) (longword_ptr - 1);

        if (cp[0] == 0)
          return cp - str;
        if (cp[1] == 0)
          return cp - str + 1;
        if (cp[2] == 0)
          return cp - str + 2;
        if (cp[3] == 0)
          return cp - str + 3;
        if (sizeof (longword) > 4)
          {
            if (cp[4] == 0)
              return cp - str + 4;
            if (cp[5] == 0)
              return cp - str + 5;
            if (cp[6] == 0)
              return cp - str + 6;
            if (cp[7] == 0)
              return cp - str + 7;
          }
      }
    }
}

int main(int argc, char const *argv[])
{
  size_t ret;
  char strTest_1[] = "HelloMy World!";
  
  ret = my_strlen(strTest_1);
  printf("strTest_1 length: %lu\r\n", ret);
  
  return 0;
}
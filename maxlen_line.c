#include <stdio.h>

#include "text_line.h"

#define MAXLEN 1000

int main() {
   int len, max;
   char line[MAXLEN];
   char longest[MAXLEN];

   max = 0;
   while ((len = get_line(line, MAXLEN)) > 0) {
       printf("%d: %s", len, line);
       if (len > max) {
           max = len;
           copy_line(longest, line);
       }
   }

   if (max > 0)
      printf("%s", longest);
   return 0;
}


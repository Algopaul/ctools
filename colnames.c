#include <stdio.h>
#include <unistd.h>
#include <ctype.h>


// Function to print the first line of an input stream that replaces a user-defined sepeartor with \n
//
int
main (int argc, char **argv)
{
  opterr = 0;
  char *svalue = NULL;
  int c;
  char *sep = ",";
  int print_col_numbers = 0;

  while ((c = getopt (argc, argv, "ns:")) != -1)
  {
    switch (c) {
      case 'n':
        print_col_numbers = 1;
        break;
      case 's':
        svalue = optarg;
        sep = svalue;
        break;
      case '?':
        if (optopt == 's')
          fprintf (stderr, "Error: option -%c requires and argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
    }
  }
  int counter = 1;
  if (print_col_numbers==1)
    printf("%2d ", counter);
  while (1) {
    int c = getchar();
    if (c == '\n' || c == EOF) {
      putchar('\n');
      break;
    }
    if (c == sep[0]) {
      printf("\n");
      if (print_col_numbers==1)
        printf("%2d ", ++counter);
    } else {
      putchar(c);
    }
  }
  return 0;
}

#https://stackoverflow.com/questions/498320/pass-arguments-into-c-program-from-command-line
#https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

char *hvalue = "Welcome to help menu...";
int opt = 0;

opterr = 0;

while ((opt = getopt(argc, argv, "h:")) != -1) //while there are remaining options
switch (opt)      
{
  case 'h':
    printf ("%s\n", hvalue);
    break;
  case '?':
      if (optopt == 'c')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort ();
}

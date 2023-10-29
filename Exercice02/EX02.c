#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096

#define USAGE_SYNTAX "[OPTIONS] -i INPUT"
#define USAGE_PARAMS "OPTIONS:\n\
  -i, --input  INPUT_FILE  : input file\n\
***\n\
  -v, --verbose : enable *verbose* mode\n\
  -h, --help    : display this help\n\
"

/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char *bin_name)
{
  dprintf(1, "USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}

/**
 * Procedure checks if variable must be free
 * (check: ptr != NULL)
 *
 * \param void* to_free pointer to an allocated mem
 * \see man 3 free
 * \return void
 */
void free_if_needed(void *to_free)
{
  if (to_free != NULL)
    free(to_free);
}

/**
 *
 * \see man 3 strndup
 * \see man 3 perror
 * \return
 */
char *dup_optarg_str()
{
  char *str = NULL;

  if (optarg != NULL)
  {
    str = strndup(optarg, MAX_PATH_LENGTH);

    if (str == NULL)
      perror(strerror(errno));
  }

  return str;
}

/**
 * Binary options declaration
 * (must end with {0,0,0,0})
 *
 * \see man 3 getopt_long or getopt
 * \see struct option definition
 */
static struct option binary_opts[] =
    {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"input", required_argument, 0, 'i'},
        {0, 0, 0, 0}};

/**
 * Binary options string
 * (linked to optionn declaration)
 *
 * \see man 3 getopt_long or getopt
 */
const char *binary_optstr = "hvi:o:";

/**
 * Binary main loop
 *
 * \return 1 if it exit successfully
 */
int main(int argc, char **argv)
{
  /**
   * Binary variables
   * (could be defined in a structure)
   */
  short int is_verbose_mode = 0;
  char *bin_input_param = NULL;

  // Parsing options
  int opt = -1;
  int opt_idx = -1;

  while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
  {
    switch (opt)
    {
    case 'i':
      // input param
      if (optarg)
      {
        bin_input_param = dup_optarg_str();
      }
      break;
    case 'v':
      // verbose mode
      is_verbose_mode = 1;
      break;
    case 'h':
      print_usage(argv[0]);
      free_if_needed(bin_input_param);
      exit(EXIT_SUCCESS);
    default:
      break;
    }
  }

  /**
   * Checking binary requirements
   * (could defined in a separate function)
   */
  if (bin_input_param == NULL)
  {
    dprintf(STDERR, "Bad usage! See HELP [--help|-h]\n");

    // Freeing allocated data
    free_if_needed(bin_input_param);
    // Exiting with a failure ERROR CODE (== 1)
    exit(EXIT_FAILURE);
  }

  // Printing params
  dprintf(1, "** PARAMS **\n%-8s: %s\n%-8s: %d\n\n",
          "input", bin_input_param,
          "verbose", is_verbose_mode);

  // Business logic must be implemented at this point
  // V1.0
  //
  // Open the source file to read
  int fd = open(bin_input_param, O_RDONLY);
  if (fd == -1)
  {
    perror("Error opening the file");
    free_if_needed(bin_input_param); // Free allocated memory
    exit(EXIT_FAILURE);              // Exit the program with an error status
  }

  // Get the source file size
  off_t file_size = lseek(fd, 0, SEEK_END);
  if (file_size == -1)
  {
    perror("Error getting file size");
    free_if_needed(bin_input_param); // Free allocated memory
    close(fd);                       // Close the file descriptor
    exit(EXIT_FAILURE);              // Exit the program with an error status
  }

  printf("Contents of the file %s in reverse order:\n", bin_input_param);

  char buffer[1];                     // Allocate a buffer to store read data
  for (off_t i = file_size - 1; i >= 0; i--)  // Loop through the file from the end to the beginning
  {
    // Move the file cursor to the specified position (i)
    if (lseek(fd, i, SEEK_SET) == -1) // SEEK_SET: start of the file
    {
      perror("Error seeking in the file");
      free_if_needed(bin_input_param); // Free allocated memory
      close(fd);                       // Close the file descriptor
      exit(EXIT_FAILURE);              // Exit the program with an error status
    }

    // Read one byte from the file into the buffer
    if (read(fd, buffer, 1) == -1)    // Read one byte from the file into the buffer
    {
      perror("Error reading the file");
      free_if_needed(bin_input_param); // Free allocated memory
      close(fd);                       // Close the file descriptor
      exit(EXIT_FAILURE);              // Exit the program with an error status
    }

    write(STDOUT_FILENO, buffer, 1);      // Write the byte from the buffer to the standard output
  }
  printf("\n");                       // Print a newline after displaying the file content
  close(fd);                          // Close the file descriptor

  free_if_needed(bin_input_param); // Free allocated memory

  return EXIT_SUCCESS;
}
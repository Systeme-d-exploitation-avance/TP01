#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define STDOUT 1
#define STDERR 2
#define MAX_PATH_LENGTH 4096

#define USAGE_SYNTAX "[OPTIONS] -d DIRECTORY"
#define USAGE_PARAMS "OPTIONS:\n\
  -d, --directory  DIRECTORY : directory to list\n\
  -v, --verbose : enable *verbose* mode\n\
  -h, --help    : display this help\n"

void print_usage(char* bin_name)
{
  dprintf(1, "USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}

void free_if_needed(void* to_free)
{
  if (to_free != NULL) free(to_free);
}

char* dup_optarg_str()
{
  char* str = NULL;

  if (optarg != NULL)
  {
    str = strndup(optarg, MAX_PATH_LENGTH);

    if (str == NULL)
      perror(strerror(errno));
  }

  return str;
}

static struct option binary_opts[] =
{
  { "help",    no_argument,       0, 'h' },
  { "verbose", no_argument,       0, 'v' },
  { "directory", required_argument, 0, 'd' },
  { 0,         0,                 0,  0  }
};

const char* binary_optstr = "hvd:";

int main(int argc, char** argv)
{
  short int is_verbose_mode = 0;
  char* bin_directory_param = NULL;

  int opt = -1;
  int opt_idx = -1;

  while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
  {
    switch (opt)
    {
      case 'd':
        if (optarg)
        {
          bin_directory_param = dup_optarg_str();
        }
        break;
      case 'v':
        is_verbose_mode = 1;
        break;
      case 'h':
        print_usage(argv[0]);
        free_if_needed(bin_directory_param);
        exit(EXIT_SUCCESS);
      default:
        break;
    }
  }

  if (bin_directory_param == NULL)
  {
    dprintf(STDERR, "Bad usage! See HELP [--help|-h]\n");
    free_if_needed(bin_directory_param);
    exit(EXIT_FAILURE);
  }

  // List files in the specified directory
  DIR* dir;
  struct dirent* ent;

  if ((dir = opendir(bin_directory_param)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      char filepath[MAX_PATH_LENGTH];
      snprintf(filepath, sizeof(filepath), "%s/%s", bin_directory_param, ent->d_name);

      struct stat file_stat;
      if (stat(filepath, &file_stat) == 0)
      {
        char permissions[11];
        permissions[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';
        permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
        permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
        permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
        permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
        permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
        permissions[10] = '\0';

        struct passwd* owner = getpwuid(file_stat.st_uid);
        struct group* group = getgrgid(file_stat.st_gid);

        char modification_time[20];
        strftime(modification_time, sizeof(modification_time), "%y/%m/%d @ %H:%M", localtime(&file_stat.st_mtime));

        dprintf(1, "%s %s %s %lld %s %s\n", permissions, owner->pw_name, group->gr_name, (long long)file_stat.st_size, modification_time, ent->d_name);
      }
    }
    closedir(dir);
  }
  else
  {
    perror("Error opening directory");
    free_if_needed(bin_directory_param);
    exit(EXIT_FAILURE);
  }

  free_if_needed(bin_directory_param);
  return EXIT_SUCCESS;
}

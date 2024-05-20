#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Simple tool for collecting and allocating jobs on a slurm cluster Example
// use: schedule -f commands.txt -p "srun --time 5" -c 10 This takes commands
// from the file commands.txt; prepends srun --time 5 and chunks together 10
// jobs.
//
// BACKGROUND: To get scheduled faster on slurm; I have divided my jobs into
// multiple sub-jobs but then my account was blocked because I submitted to
// many small jobs; so this now chunks small jobs back together so I can launch
// ca. 30 minute long jobs, which is what the admin wanted me to do.
//
// Also: When I submit more than 1000~2000 jobs; my jobs are not scheduled at
// all and just crash so I implement a `submit_or_wait` function

// TODO: implement check that ensures total length of commands is not longer
// than previously alloted

int number_of_jobs(char * user, int only_running) {
  FILE *fp;
  char path[1035];
  int jobs=0;
  char slurm_cmd[256];
  snprintf(slurm_cmd, sizeof(slurm_cmd), "squeue -u %s -h -o %T", user);
  fp = popen(slurm_cmd, "r");
  if (fp == NULL) {
    printf("Failed to run command\n");
    exit(1);
  }

  while (fgets(path, sizeof(path) - 1, fp) != NULL) {
    if (only_running == 1 && path[0] == 'R') {
      jobs++;
    } else if (only_running == 0) {
      jobs++;
    }
  }
  pclose(fp);

  return jobs;
}

int submit_or_wait(char * precommand, char * accumulated, char * user, int max_jobs, int chunk_size) {
  int njobs=number_of_jobs(user, 0);
  printf("Njobs: %d\n", njobs);
  char execute[2650];
  snprintf(execute, 2650, "%s %s", precommand, accumulated);
  while (1) {
    if (njobs+chunk_size < max_jobs) {
      popen(execute, "r");
      sleep(0.5);
      break;
    } else {
      printf("Too many jobs; try again in 10 seconds.\n");
      sleep(10);
    }
    njobs=number_of_jobs(user, 0);
  }
  return 0;
}

int main(int argc, char **argv) {
  int c;
  int chunk_size=1;
  FILE *commands;
  size_t len;
  size_t read;
  char * line=NULL;
  char precommand[256];
  char accumulated[2048];
  char filename[1024];
  char user[32];
  snprintf(user, sizeof(user), getenv("USER"));
  int max_jobs=1000;

  while ((c = getopt(argc, argv, "c:p:f:")) != -1 ) {
    switch (c) {
      case 'p':
        snprintf(precommand, sizeof(precommand), "%s", optarg);
        break;
      case 'c':
        chunk_size = atoi(optarg);
        break;
      case 'f':
        snprintf(filename, sizeof(filename), "%s", optarg);
        break;
    }
  }
  printf("%d\n", chunk_size);


  commands = fopen(filename, "r");
  if (commands == NULL)
    exit(EXIT_FAILURE);

  int chunk_counter=0;
  int accumulated_size=0;

  while ((read = getline(&line, &len, commands)) != -1) {
    chunk_counter++;
    snprintf(&accumulated[accumulated_size], read, "%s", line);
    snprintf(&accumulated[accumulated_size+read-1], 3, ";  ");
    accumulated_size=accumulated_size+(int)read+1;
    if (chunk_counter % chunk_size == 0) {
      accumulated[accumulated_size] = '\0';
      submit_or_wait(precommand, accumulated, user, max_jobs, chunk_size);
      accumulated_size=0;
    }
  }
  accumulated[accumulated_size] = '\0';
  printf("%s %s\n", precommand, accumulated);
  accumulated_size=0;

  return 0;
}

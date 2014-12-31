#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <dlfcn.h>

#include "argParser.h"
#include "sorting_lib.h"

unsigned ipow(unsigned base, unsigned exp)
{
  if(exp == 0) return 1;
  unsigned i;
  unsigned tmp = base;
  for(i = 1; i < exp; i++) tmp = tmp * base;
  return tmp;
}


static int profileSwaps = 0;

static unsigned long long *pTotalSwaps = 0;

static int profileMemory = 0;
static int recordMemory = 0;

static size_t totalAllocations = 0;

static void* (*o_malloc)(size_t) = 0;
static void* (*o_realloc)(void*, size_t) = 0;
static void* (*o_calloc)(size_t, size_t) = 0;
//static void  (*o_free)(void*) = 0;


void *malloc(size_t size)
{
  if(!o_malloc) o_malloc = dlsym(RTLD_NEXT, "malloc");
  if(profileMemory && recordMemory)
  {
    totalAllocations += size;
    //printf("Allocated %llu\n", (unsigned long long)size);
  }
  return o_malloc(size);
}

void *realloc(void* ptr, size_t size)
{
  if(!o_realloc) o_realloc = dlsym(RTLD_NEXT, "realloc");
  if(profileMemory && recordMemory)
  {
    totalAllocations += size;
    //printf("Allocated %llu\n", (unsigned long long)size);
  }
  return o_realloc(ptr, size);
}

void *calloc(size_t nmemb, size_t size)
{
  if(!o_calloc) o_calloc = dlsym(RTLD_NEXT, "calloc");
  if(profileMemory && recordMemory)
  {
    totalAllocations += size * nmemb;
    //printf("Allocated %llu\n", (unsigned long long)size);
  }
  return o_calloc(nmemb, size);
}

static unsigned long long runCompares = 0;

int intCompare(void* a, void* b)
{
  runCompares++;
  int x = *((int*)a), y = *((int*)b);
  return (x < y)?-1:((x > y)?1:0);
}

int isSortedIntegers(int *numbers, size_t n)
{
  int i;
  int last = numbers[0];
  for(i = 1; i < n; i++)
  {
    if(last > numbers[i]) return 0;
    last = numbers[i];
  }
  return 1;
}

void testIntegerSorting(sortFn_t f, int *numbers, size_t n, FILE* output)
{
  //int i;
  runCompares = 0;
  totalAllocations = 0;
  if(pTotalSwaps) *pTotalSwaps = 0;
  recordMemory = 1;
  clock_t t = clock();
  f((void*)numbers, n, sizeof(int), intCompare);
  t = clock() - t;
  recordMemory = 0;
  int valid = isSortedIntegers(numbers, n);
  printf("%10llu %10llu %10llu %10llu %10.04lfms \e[38;5;%um%10s\e[0m\n",
         (unsigned long long)n,
         runCompares,
         (pTotalSwaps)?*pTotalSwaps:0,
         (profileMemory)?(unsigned long long)totalAllocations:0,
         ((double)t * 1000)/CLOCKS_PER_SEC,
         valid?82:160,
         valid?"valid":"invalid");
  if(output) fprintf(output, "%llu %lf %llu %llu %llu\n",
                             (unsigned long long)n, ((double)t * 1000)/CLOCKS_PER_SEC,
                             runCompares,
                             (pTotalSwaps)?*pTotalSwaps:0,
                             (profileMemory)?(unsigned long long)totalAllocations:0);
  //printf("%llu\n", (unsigned long long)totalAllocations);
  //for(i = 0; i < n; i++) printf("%d\n", numbers[i]);
  //free(numberList);  
}

unsigned calculateSortSize(unsigned sortSize0, unsigned sample, unsigned growth, char growthType)
{
  unsigned tmp = 0;
  switch(growthType)
  {
    case 1:
      tmp = (sortSize0 * (sample * growth)); break;
    case 2: 
      tmp = (sortSize0 * ipow(growth, sample)); break;
    case 3:
      tmp = sortSize0 + (sortSize0 * log10(growth*sample)); break;
  }

  return tmp;
}

int main(int argc, char **argv)
{
  char outputPlotData = 0;
  char *plotFolder;

  char *moduleFolder = malloc(3);
  strcpy(moduleFolder, "./");

  unsigned sortSize0 = 10, sortSize;
  unsigned runs = 5;

  unsigned runSortSizeGrowthRate = 2;
  unsigned runSortSizeGrowthType = 1;

  int profileSwaps0 = 0;


  ArgList_t *pargs = arg_initArgs(argc, argv);

  ArgParam_t *aplot = arg_addParam(pargs, 'p', "plot");
  ArgParam_t *amodules =   arg_addParam(pargs, 'l', "libs");
  ArgParam_t *asortsize = arg_addParam(pargs, 's', "start-size");
  ArgParam_t *aruns = arg_addParam(pargs, 'r', "runs");
  ArgParam_t *agrowth = arg_addParam(pargs, 'g', "growth");
  ArgParam_t *agrowthtype = arg_addParam(pargs, 't', "growth-type");
  ArgSwitch_t *aprofilemem = arg_addSwitch(pargs, 'm', "profile-memory"); 
  ArgSwitch_t *aprofileswaps = arg_addSwitch(pargs, 'n', "profile-swaps");

  arg_parseArgs(pargs);

  if(aplot->value && strlen(aplot->value))
  {
    outputPlotData = 1;
    plotFolder = aplot->value;
  }

  if(amodules->value && strlen(amodules->value))
  {
    free(moduleFolder);
    moduleFolder = malloc(strlen(amodules->value) + 1);
    strcpy(moduleFolder, amodules->value);
  }

  if(asortsize->value && strlen(asortsize->value))
  {
    sscanf(asortsize->value, "%u", &sortSize0);
  }

  if(aruns && aruns->value && strlen(aruns->value))
  {
    sscanf(aruns->value, "%u", &runs);
  }

  if(agrowth->value && strlen(agrowth->value))
  {
    sscanf(agrowth->value, "%u", &runSortSizeGrowthRate);
  }

  if(agrowthtype->value && strlen(agrowthtype->value))
  {
    sscanf(agrowthtype->value, "%u", &runSortSizeGrowthType);
  }

  if(aprofilemem->switched) profileMemory = 1;
  if(aprofileswaps->switched) 
  {
    profileSwaps0 = 1;
    profileSwaps = 1;
  }

  arg_destroyArgs(pargs);

  unsigned maxSortSize = calculateSortSize(sortSize0, runs, runSortSizeGrowthRate, runSortSizeGrowthType);

  printf("Runs: %u\nMin. Values: %u\nGrowth: %u\nGrowth-type: %u\nMax. Values: %u\n", runs, sortSize0, runSortSizeGrowthRate, runSortSizeGrowthType, maxSortSize);
  
  time_t tnow = time(0);
  struct tm *now = localtime(&tnow);
  char timeDate[16];
  strftime(timeDate, 16, "%d%m%Y_%H%M%S", now);  

  char strtmp[128];
  FILE *pPlotFile = 0;
  FILE *pPlotFileMem = 0;
  FILE *pPlotFileSwap = 0;
  FILE *pPlotFileComp = 0;

  if(outputPlotData)
  {
    snprintf(strtmp, 127, "%s/sorts_time_%s.gp", plotFolder, timeDate);
    pPlotFile = fopen(strtmp, "w");
    if(!pPlotFile)
    {
      perror("Opening Plot-file failed!");
      return 1;
    }
    fprintf(pPlotFile, "set title \"Sorting Algorithms Time Benchmark\"\n"
                       "set xlabel \"Worksize(Array-elements)\"\n"
                       "set ylabel \"Time(ms)\"\n"
                       "set autoscale\n"
                       "plot ");

    snprintf(strtmp, 127, "%s/sorts_compares_%s.gp", plotFolder, timeDate);
    pPlotFileComp = fopen(strtmp, "w");
    if(!pPlotFileComp)
    {
      perror("Opening Plot-file failed!");
      return 1;
    }
    fprintf(pPlotFileComp, "set title \"Sorting Algorithms Comparisons Benchmark\"\n"
                       "set xlabel \"Worksize(Array-elements)\"\n"
                       "set ylabel \"Comparisons\"\n"
                       "set autoscale\n"
                       "plot ");

    if(profileMemory)
    {
      snprintf(strtmp, 127, "%s/sorts_memory_%s.gp", plotFolder, timeDate);
      pPlotFileMem = fopen(strtmp, "w");
      if(!pPlotFileMem)
      {
        perror("Opening Plot-file failed!");
        return 1;
      }
      fprintf(pPlotFileMem, "set title \"Sorting Algorithms Memory Benchmark\"\n"
                         "set xlabel \"Worksize(Array-elements)\"\n"
                         "set ylabel \"Memory Usage\"\n"
                         "set autoscale\n"
                         "plot ");
    }

    if(profileSwaps)
    {
      snprintf(strtmp, 127, "%s/sorts_swaps_%s.gp", plotFolder, timeDate);
      pPlotFileSwap = fopen(strtmp, "w");
      if(!pPlotFileSwap)
      {
        perror("Opening Plot-file failed!");
        return 1;
      }
      fprintf(pPlotFileSwap, "set title \"Sorting Algorithms Swaps Benchmark\"\n"
                         "set xlabel \"Worksize(Array-elements)\"\n"
                         "set ylabel \"Swaps\"\n"
                         "set autoscale\n"
                         "plot ");
    }
  }

  DIR *modDir = opendir(moduleFolder);
  //free(moduleFolder);
  if(!modDir)
  {
    perror("Opening module directory failed!");
    if(outputPlotData)
    {
      fclose(pPlotFile);
      remove(strtmp);
    }
    free(moduleFolder);
    return 1;
  }

  //test array for sorting numbers of random order
  int *randomNumbers0 = malloc(maxSortSize * sizeof(int)); //original random list
  int *randomNumbers = malloc(maxSortSize * sizeof(int)); //copy to be sorted
  unsigned long long i;
  srand(time(0));
  for(i = 0; i < maxSortSize; i++)
  {
    randomNumbers[i] = randomNumbers0[i] = rand();
  }

  //memcpy(randomNumbers, randomNumbers0, sizeof(int)*maxSortSize);

  int *sortedNumbers0 = malloc(maxSortSize * sizeof(int));
  int *sortedNumbers = malloc(maxSortSize * sizeof(int));
  for(i = 0; i < maxSortSize; i++)
  {
    sortedNumbers[i] = sortedNumbers0[i] = i;
  }

  char plotDataName[128];

  struct dirent *file;
  void *libHandle = 0;
  getSortNameFn_t sortNameFn = 0;
  getSortSymbolFn_t sortSymbolFn = 0;
  sortFn_t sortFn = 0;
  while((file = readdir(modDir)))
  {
    if(file->d_type & DT_REG && strstr(file->d_name, ".so"))
    {
        char *fullPath = malloc(strlen(file->d_name) + strlen(moduleFolder) + 1);
        /*
        fullPath[0] = '.';
        fullPath[1] = '/';
        fullPath[2] = 0;*/
        fullPath[0] = 0;
        strcpy(fullPath, moduleFolder);
        strcat(fullPath, file->d_name);
        libHandle = dlopen(fullPath, RTLD_LAZY);
        if(!libHandle)
        {
          fprintf(stderr, "Loading \"%s\" failed!(%s)\n", fullPath, dlerror());
          free(fullPath);
          continue;
        }
        free(fullPath);

        sortNameFn = (getSortNameFn_t)dlsym(libHandle, "getSortName");
        if(!sortNameFn) continue;

        sortSymbolFn = (getSortSymbolFn_t)dlsym(libHandle, "getSortSymbol");
        if(!sortSymbolFn)
        {
          fprintf(stderr, "Can't find procedure!(%s)\n", dlerror());
          continue;
        }

        sortFn = (sortFn_t)dlsym(libHandle, sortSymbolFn());
        if(!sortFn)
        {
          fprintf(stderr, "Can't find sort procedure!(%s)\n", dlerror());
          continue;
        }

        pTotalSwaps = dlsym(libHandle, "totalSwaps");
        if(profileSwaps0 && pTotalSwaps)
        {
          printf("Profiling swaps.\n");
          profileSwaps = 1;
        }
        else
        {
          profileSwaps = 0;
        }

        printf("Testing %s\n", sortNameFn());
        printf("Pre-Sorted:\n");
        printf("%10s %10s %10s %10s %12s %10s\n", "Values", "Compares", "Swaps", "Allocs", "Time", "Validity");

        FILE* plotData = 0;

        if(outputPlotData)
        {
          snprintf(plotDataName, 127, "%s_sorted_%s.gpd", sortNameFn(), timeDate);
          snprintf(strtmp, 127, "%s/%s", plotFolder, plotDataName);
          plotData = fopen(strtmp, "w");
        }

        for(i = 0; i < runs; i++)
        {
          sortSize = calculateSortSize(sortSize0, i+1, runSortSizeGrowthRate, runSortSizeGrowthType);
          testIntegerSorting(sortFn, sortedNumbers, sortSize, plotData);
          memcpy(sortedNumbers, sortedNumbers0, sizeof(int)*sortSize); //reset the sorted parts for the next run
        }

        if(outputPlotData)
        {
          fclose(plotData);
          fprintf(pPlotFile, "\"%s\" u 1:2 t \"%s Time Sorted\" w points, ", plotDataName, sortNameFn());
          fprintf(pPlotFileComp, "\"%s\" u 1:3 t \"%s Comparisons Sorted\" w points,", plotDataName, sortNameFn());
          if(profileMemory && pPlotFileMem) fprintf(pPlotFileMem, "\"%s\" u 1:5 t \"%s Sorted\" w points, ", plotDataName, sortNameFn());
          if(profileSwaps && pPlotFileSwap)  fprintf(pPlotFileSwap, "\"%s\" u 1:4 t \"%s Sorted\" w points, ", plotDataName, sortNameFn());
        }

        printf("Random:\n");
        printf("%10s %10s %10s %10s %12s %10s\n", "Values", "Compares", "Swaps", "Allocs", "Time", "Validity");

        if(outputPlotData)
        {
          snprintf(plotDataName, 127, "%s_random_%s.gpd", sortNameFn(), timeDate);
          snprintf(strtmp, 127, "%s/%s", plotFolder, plotDataName);
          plotData = fopen(strtmp, "w");
        }

        for(i = 0; i < runs; i++)
        {
          sortSize = calculateSortSize(sortSize0, i+1, runSortSizeGrowthRate, runSortSizeGrowthType);
          //*totalSwaps = 0;
          testIntegerSorting(sortFn, randomNumbers, sortSize, plotData);
          //printf("%llu\n", *totalSwaps);
          memcpy(randomNumbers, randomNumbers0, sizeof(int)*sortSize); //reset the sorted parts for the next run
        }

        if(outputPlotData)
        {
          fclose(plotData);
          fprintf(pPlotFile, "\"%s\" u 1:2 t \"%s Time Random\" w points, ", plotDataName, sortNameFn());
          fprintf(pPlotFileComp, "\"%s\" u 1:3 t \"%s Comparisons Random\" w points,", plotDataName, sortNameFn());
          if(profileMemory && pPlotFileMem) fprintf(pPlotFileMem, "\"%s\" u 1:5 t \"%s Random\" w points, ", plotDataName, sortNameFn());
          if(profileSwaps && pPlotFileSwap)  fprintf(pPlotFileSwap, "\"%s\" u 1:4 t \"%s Random\" w points, ", plotDataName, sortNameFn());
        }
       
        dlclose(libHandle);
        sortFn = 0;
        sortNameFn = 0;
        sortSymbolFn = 0;
        pTotalSwaps = 0;
    }
  }
  closedir(modDir);
  if(outputPlotData)
  {
    fclose(pPlotFile);
    fclose(pPlotFileComp);
    if(profileMemory) fclose(pPlotFileMem);
    if(profileSwaps0)  fclose(pPlotFileSwap);
  }
  
  free(moduleFolder);
  free(randomNumbers);
  free(randomNumbers0);
  free(sortedNumbers);
  free(sortedNumbers0);

  return 0;
}

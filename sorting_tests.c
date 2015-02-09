/**
 * @file sorting_tests.c
 * @author: Roy Freytag
 *
 * Basic benchmark for sorting algorithms
 */

//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <dlfcn.h>

#include "argParser.h"
#include "sorting_lib.h"

//variables we'll need in some functions
static unsigned int averagingRuns = 3; ///< how often to run a test on one sample size to average out

static int profileSwaps = 0; ///< decides whether to profile swaps or not
static unsigned long long *pTotalSwaps = 0; ///< pointer to Swap counter

static int profileMemory = 0; ///< decides whether to profile memory allocations or not
static int recordMemory = 0; ///< set to one when memory is supposed to be recorded

static size_t totalAllocations = 0; ///< memory allocations counter in byte

static unsigned long long runCompares = 0; ///< comparisons counter

//We can only profile memory if we use the GNU C Standard-lib as of now
#ifdef _GNU_SOURCE
//store original function-pointers, to call later on
static void* (*o_malloc)(size_t) = 0; ///< function-pointer to original malloc()
static void* (*o_realloc)(void*, size_t) = 0; ///< function-pointer to original realloc()
static void* (*o_calloc)(size_t, size_t) = 0; ///< function-pointer to original calloc()
//static void  (*o_free)(void*) = 0;

/**
 * @brief redefinition of malloc().
 * Will count allocated bytes.
 * @param size allocation size.
 */
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

/**
 * @brief redefinition of realloc().
 * @see malloc()
 * @param ptr pointer to memory block that should be resized.
 * @param size allocation size.
 *
 * @todo don't recount previously allocated memory in ptr, should relatively overhead free though.
 */
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

/**
 * @brief redefinition of calloc().
 * @see malloc()
 * @param nmemb member size.
 * @param size allocation size.
 */
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

#endif

/**
 * @brief Simple Power-Of for unsigned integers
 * @param base
 * @param exp
 *
 * @return base^exp
 */
unsigned ipow(unsigned base, unsigned exp)
{
  if(exp == 0) return 1;
  unsigned i;
  unsigned tmp = base;
  for(i = 1; i < exp; i++) tmp = tmp * base;
  return tmp;
}

/**
 * @brief Comparison function for two integers
 * @param a generic pointer to left-hand integer.
 * @param b generic pointer to right-hand integer.
 * @return
 * - -1 when a < b
 * - 0 if a == b
 * - 1 if a > b
 */
int intCompare(void* a, void* b)
{
  runCompares++;
  int x = *((int*)a), y = *((int*)b);
  return (x < y)?-1:((x > y)?1:0);
}

/**
 * @brief Quick check if list is sorted.
 * Quickly checks if list is sorted decendingly.
 *
 * @param numbers pointer to array.
 * @param n array size.
 * @return
 * - 1 if it's sorted
 * - 0 otherwise
 *
 * @todo check if all numbers from the original list are still there.
 */
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

/**
 * @brief commences sorting tests.
 *
 * Takes the inputed array and sorts it with the given sorting function.
 * If there are more than one runs to do, there will be copies made of the original list, so each run gets exactly the same unsorted list.
 * If there is a pointer to a swap-counter, the swap-count will be reset to zero, all other counters are reset as well.
 * @param f function-pointer of sorting function.
 * @param numbers pointer to original array.
 * @param n size of array.
 * @param output file to write the recorded data to.
 */
void testIntegerSorting(sortFn_t f, int *numbers, size_t n, FILE* output)
{
  unsigned int i;

  int *snumbers = numbers;

  if(averagingRuns > 0)
  {
    snumbers = malloc(sizeof(int) * n);
    memcpy(snumbers, numbers, sizeof(int) * n);
  }

  runCompares = 0;
  totalAllocations = 0;
  if(pTotalSwaps) *pTotalSwaps = 0;

  unsigned long long o_runCompares = 0;
  size_t o_totalAllocations = 0;
  unsigned long long o_totalSwaps = 0;

  //for(i = 0; i < n; i++) printf("%d\n", numbers[i]);
  double time = 0.f;
  for(i = 0; i < averagingRuns; i++)
  {
    memcpy(snumbers, numbers, sizeof(int) * n);
    clock_t t = clock();
    recordMemory = 1;
    f((void*)snumbers, n, sizeof(int), intCompare);
    recordMemory = 0;
    t = clock() - t;
    time += ((double)t * 1000)/CLOCKS_PER_SEC;

    //record these things only once, as they will be constant anyways
    if(i == 0)
    {
      o_runCompares = runCompares;
      o_totalAllocations = totalAllocations;
      if(pTotalSwaps) o_totalSwaps = *pTotalSwaps;
    }

    //reset for next run
    runCompares = 0;
    totalAllocations = 0;
    if(pTotalSwaps) *pTotalSwaps = 0;
  }

  if(averagingRuns)
  {
    time = time / averagingRuns;  
  }

  int valid = isSortedIntegers(snumbers, n);
  printf("%10llu %10llu %10llu %10llu %10.04lfms \e[38;5;%um%10s\e[0m\n",
         (unsigned long long)n,
         o_runCompares,
         o_totalSwaps,
         (profileMemory)?(unsigned long long)o_totalAllocations:0,
         time,
         valid?82:160,
         valid?"valid":"invalid");
  if(output) fprintf(output, "%llu %lf %llu %llu %llu\n",
                             (unsigned long long)n, 
                             time,
                             runCompares,
                             (pTotalSwaps)?*pTotalSwaps:0,
                             (profileMemory)?(unsigned long long)totalAllocations:0);
  free(snumbers);
  //printf("%llu\n", (unsigned long long)totalAllocations);
  //for(i = 0; i < n; i++) printf("%d\n", numbers[i]);
  //free(numberList);  
}

/**
 * @brief Helper Function to calculate the work-sizes.
 *
 * The following growth types are supported as of now:
 * - 1: (sortSize0 * (sample * growth))
 * - 2: (sortSize0 * ipow(growth, sample))
 * - 3: sortSize0 + (sortSize0 * log10(growth*sample))
 *
 * @param sortSize0 initial work-size.
 * @param sample the sample/run number.
 * @param growth growth from sample to sample.
 * @param growthType type of growth.
 */
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

/**
 * @brief prints help.
 * @param cmd own name
 */
void printHelp(char *cmd)
{
  printf("Usage:\n\t%s [<options>]\n", cmd);
  printf("Available Options:\n"
         "\t-l,--libs <folder>         - Folder in which the libraries of the sorting algorithms to be tested are stored.\n"
         "\t-p,--plots <folder>        - Folder in which the plot data will be written.\n"
         "\t-s,--start-size <number>   - Start-value to base the sample size on.\n"
         "\t-r,--runs <number>         - number of test runs to perform.\n"
         "\t-g,--growth <number>       - run to run growth.\n"
         "\t-t,--growth-type <number>  - how the sample size will grow.(1: linear, 2: exponential, 3: logarithmic)\n"
         "\t-m,--profile-memory        - record memory usage.\n"
         "\t-n,--profile-swaps         - record how many swaps were needed.\n"
         "\t-v,--verbose               - output lists.\n"
         "\t-h,--help                  - this.\n"
         "\t-a,--average <number>      - how often to run the test to average the time.\n");
}

int main(int argc, char **argv)
{
  //some variables we will need
  char outputPlotData = 0;
  char *plotFolder;

  char *moduleFolder = malloc(3);
  strcpy(moduleFolder, "./");

  unsigned sortSize0 = 10, sortSize;
  unsigned runs = 5;

  unsigned runSortSizeGrowthRate = 2;
  unsigned runSortSizeGrowthType = 1;

  int profileSwaps0 = 0;


  //create Argument List
  ArgList_t *pargs = arg_initArgs(argc, argv);
  //add parameters and switches
  ArgParam_t *aplot = arg_addParam(pargs, 'p', "plot");
  ArgParam_t *amodules =   arg_addParam(pargs, 'l', "libs");
  ArgParam_t *asortsize = arg_addParam(pargs, 's', "start-size");
  ArgParam_t *aruns = arg_addParam(pargs, 'r', "runs");
  ArgParam_t *agrowth = arg_addParam(pargs, 'g', "growth");
  ArgParam_t *agrowthtype = arg_addParam(pargs, 't', "growth-type");
  ArgParam_t *aaveraging = arg_addParam(pargs, 'a', "average");
  ArgSwitch_t *aprofilemem = arg_addSwitch(pargs, 'm', "profile-memory"); 
  ArgSwitch_t *aprofileswaps = arg_addSwitch(pargs, 'n', "profile-swaps");
  ArgSwitch_t *averbose = arg_addSwitch(pargs, 'v', "verbose");
  ArgSwitch_t *ahelp = arg_addSwitch(pargs, 'h', "help");

  //parse arguments from args into the previously defined switches and parameters
  arg_parseArgs(pargs);

  //get the data

  if(ahelp->switched)
  {
    printHelp(argv[0]);
    free(moduleFolder);
    return 0;
  }

  if(aplot->value && strlen(aplot->value))
  {
    outputPlotData = 1;
    plotFolder = aplot->value;
    printf("Will output plots to \"%s\".\n", plotFolder);
  }

  if(amodules->value && strlen(amodules->value))
  {
    free(moduleFolder);
    moduleFolder = malloc(strlen(amodules->value) + 1);
    strcpy(moduleFolder, amodules->value);
    printf("Will search in \"%s\" for modules.\n", moduleFolder);
  }

  if(asortsize->value && strlen(asortsize->value))
  {
    sscanf(asortsize->value, "%u", &sortSize0);
  }

  if(aruns && aruns->value && strlen(aruns->value))
  {
    sscanf(aruns->value, "%u", &runs);
  }

  if(aaveraging && aaveraging->value && strlen(aaveraging->value))
  {
    sscanf(aaveraging->value, "%u", &averagingRuns);
  }

  if(agrowth->value && strlen(agrowth->value))
  {
    sscanf(agrowth->value, "%u", &runSortSizeGrowthRate);
  }

  if(agrowthtype->value && strlen(agrowthtype->value))
  {
    sscanf(agrowthtype->value, "%u", &runSortSizeGrowthType);
  }

  if(aprofilemem->switched)
  {
    profileMemory = 1;
#ifdef _GNU_SOURCE
    printf("Will profile memory usage.\n");
#else
    printf("Can't profile memory usage!\n");
#endif
  }
  if(aprofileswaps->switched) 
  {
    profileSwaps0 = 1;
    profileSwaps = 1;

    printf("Will profile swaps.\n");
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

  //get our GNU Plot script ready
  if(outputPlotData)
  {
    snprintf(strtmp, 127, "%s/sorts_time_%s.gp", plotFolder, timeDate);
    pPlotFile = fopen(strtmp, "w");
    if(!pPlotFile)
    {
      perror("Opening Plot-file failed!");
      free(moduleFolder);

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
      free(moduleFolder);
      fclose(pPlotFile);
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
        free(moduleFolder);
        fclose(pPlotFile);
        fclose(pPlotFileComp);
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
        free(moduleFolder);
        fclose(pPlotFile);
        fclose(pPlotFileComp);
        if(profileMemory) fclose(pPlotFileMem);
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
      fclose(pPlotFileComp);
      if(profileMemory) fclose(pPlotFileMem);
      if(profileSwaps) fclose(pPlotFileSwap);
      //remove(strtmp);
    }
    free(moduleFolder);
    return 1;
  }

  //test array for sorting numbers of random order
  //int *randomNumbers0 = malloc(maxSortSize * sizeof(int)); //original random list
  int *randomNumbers = malloc(maxSortSize * sizeof(int)); //copy to be sorted
  if(!randomNumbers)
  {
    perror("Couldn't allocate random number array!");
    if(outputPlotData)
    {
      fclose(pPlotFile);
      fclose(pPlotFileComp);
      if(profileMemory) fclose(pPlotFileMem);
      if(profileSwaps) fclose(pPlotFileSwap);
      //remove(strtmp);
    }
    free(moduleFolder);
    return 1;
  }

  unsigned long long i;
  srand(time(0));
  for(i = 0; i < maxSortSize; i++)
  {
    randomNumbers[i] = rand();//randomNumbers0[i] = rand();
  }

  //memcpy(randomNumbers, randomNumbers0, sizeof(int)*maxSortSize);

  //int *sortedNumbers0 = malloc(maxSortSize * sizeof(int));
  int *sortedNumbers = malloc(maxSortSize * sizeof(int));
  if(!sortedNumbers)
  {
    perror("Couldn't allocate random number array!");
    if(outputPlotData)
    {
      fclose(pPlotFile);
      fclose(pPlotFileComp);
      if(profileMemory) fclose(pPlotFileMem);
      if(profileSwaps) fclose(pPlotFileSwap);
      //remove(strtmp);
    }
    free(moduleFolder);
    free(randomNumbers);
    return 1;
  }

  for(i = 0; i < maxSortSize; i++)
  {
    sortedNumbers[i] = i; //sortedNumbers0[i] = i;
  }

  char plotDataName[128];

  struct dirent *file;
  void *libHandle = 0;
  getSortNameFn_t sortNameFn = 0;
  getSortSymbolFn_t sortSymbolFn = 0;
  sortFn_t sortFn = 0;
  //open the folder and search for .so modules
  while((file = readdir(modDir)))
  {
    if(file->d_type & DT_REG && strstr(file->d_name, ".so"))
    {
      //found a module, so lets try opening it
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
        free(randomNumbers);
        free(sortedNumbers);
        continue;
      }
      free(fullPath);

      sortNameFn = (getSortNameFn_t)dlsym(libHandle, "getSortName");
      if(!sortNameFn)
      {
        dlclose(libHandle);
        continue;
      }

      sortSymbolFn = (getSortSymbolFn_t)dlsym(libHandle, "getSortSymbol");
      if(!sortSymbolFn)
      {
        fprintf(stderr, "Can't find procedure!(%s)\n", dlerror());
        sortNameFn = 0;
        dlclose(libHandle);
        continue;
      }

      sortFn = (sortFn_t)dlsym(libHandle, sortSymbolFn());
      if(!sortFn)
      {
        fprintf(stderr, "Can't find sort procedure!(%s)\n", dlerror());
        sortNameFn = 0;
        sortSymbolFn = 0;
        dlclose(libHandle);
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
        //memcpy(sortedNumbers, sortedNumbers0, sizeof(int)*sortSize); //reset the sorted parts for the next run
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
        //memcpy(randomNumbers, randomNumbers0, sizeof(int)*sortSize); //reset the sorted parts for the next run
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
  //free(randomNumbers0);
  free(sortedNumbers);
  //free(sortedNumbers0);

  return 0;
}

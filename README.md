# SortBenchmark
A simple little tools to benchmark sort algorithms and output a gnuplot script to visualize the results.

## Benchmarking

To Benchmark a sorting algorithm you simply compile a dynamic library containing the sort function and 
two extra functions returning the name and the symbol name of the sort function.

An example is given in sorts/qsort/

# Sort Modules

The Sort module will be loaded in order to commence the benchmark.

## Requirements

The module needs to have two functions at least:

```
char* getSortName(void); //Name of sorting algorithm to be displayed
char* getSortSymbol(void); //Symbol name of the sort function, e.g. "qsort"
```

The Sort function must have the following function signature

```
void <function name>(void*, size_t, size_t, int (*)(void*,void*))
```

The function name is freely choosable, but you have to return that chosen function name as a string in getSortSymbol().

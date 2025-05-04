# Cache-Simulator
Cache simulator with cache L1, set-associative and victim cache associative

## Table of Contents
  1. [General information](#general-information)
  2. [Installation](#installation,-compilation-and-execute)
  3. [Use in any project](#use-in-any-project)

### General information
Specification of the simulator (simulator.c):
  - Cache (L1 and set-associative)
  - Victim cache (associative)
  - Config file (config): Size cache and size of the lines in bytes, number of ways in sets and the number of lines in victim cache (0 => There isn't victim cache).

Specification of the trace (trace module):
  - init
  - write/read
  - close
  - Objetive: In any program, it module can used for write the trace of any variable/s.

Specification of the test/:
  - All programs in test/ are examples about use trace module. Also contains tecniques for leverage the memory cache which dummy variables, best algorithms...

Specification of the makefile:
  - The makefile can compile the simulator, program_1, program_2 and program3 in test.

Objetive:
  - This cache simulator calculate with trace_file generated the miss rate, hit rate, number of access to cache and number of miss.

### Installation, compilation and execute:
  - For clone: ``` git clone https://github.com/eloyUA/Cache-Simulator ```
  - For compile any program in test: ``` make test_program_i ```
  - For compile simulator: ``` make ``` or ``` make simulator ```
  - For execute program_i: ``` ./program_i ``` => Trace file is generated (for example "test/trace_i")
  - For execute simulator: ``` ./simulator <name_trace_file> ``` so ``` ./simulator test/trace_i ```

### Use in any project
1) Use "trace.h" in your program for generate trace_file.
2) Execute simulator with the trace_file.

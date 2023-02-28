# xv6-utils

Command-line utilities written in C for the xv6 teaching operating system. Made as part of a
[course on operating systems](https://www.ntnu.edu/studies/courses/TDT4186) at NTNU Trondheim,
spring 2023.

## Project Structure

- `lib` contains the xv6 teaching operating system developed by MIT, for the RISC-V architecture
- `src` contains this project's extensions to xv6
  - `hello` is a simple user-space program that greets the user
  - `processes` contains programs and utilities for getting information about running processes in
    the system
    - `ps` is a user-space program that prints all currently running processes, along with their
      status code
    - `proctree` is another user-space program that prints the hierarchy of running processes,
      listing child processes under their parents
    - `sys_getprocs` implements a new syscall `getprocs`, which returns information about a given
      number of running processes from the kernel
    - `process_list` provides a dynamically sized list of all currently running processes, filled by
      calling `getprocs` in batches, and used by the `ps` and `proctree` programs
  - `scheduling` contains a scheduler: a kernel-space algorithm that decides which processes get to
    execute at a time
    - `mlfq_scheduler` implements a Multi-Level Feedback Queue (MLFQ) scheduler for xv6
  - `utils` contains general-use utility functions and data types
    - `int_list` provides a dynamically sized list of integers
    - `realloc` provides a function to reallocate memory to a new size
    - `strings` provides operations on strings

# Cache Management System

## Overview
This project implements a cache management system designed to store, retrieve, and manage objects in a hash table. 
The system supports dynamic resizing through rehashing, collision resolution via quadratic probing, and the efficient 
removal of outdated or deleted entries. It incorporates a dual-table structure to facilitate incremental rehashing for performance optimization.

---

## File Descriptions

1. **`cache.h`**
   - Header file that defines the **Cache**, **Person**, and supporting classes.
   - **Key Classes**:
     - **Person**: Represents a data object with attributes `key` (string) and `id` (integer).
     - **Cache**: Implements the core functionality of a hash table, including insertion, deletion, rehashing, and lookup.
   - **Key Features**:
     - Quadratic probing for collision resolution.
     - Rehashing triggered by load factor (`lambda() > 0.5`) or deleted ratio (`deletedRatio() > 0.8`).
     - Dual-table architecture for seamless transitioning during rehashing.

2. **`cache.cpp`**
   - Implements the methods declared in `cache.h`.
   - **Key Implementations**:
     - **Insert**: Adds objects to the hash table while maintaining consistency during collisions and rehashing.
     - **Remove**: Deletes objects and updates metadata to reflect the changes.
     - **Rehashing**: Dynamically resizes the hash table based on usage patterns and performance metrics.
     - Helper methods for checking prime numbers and incremental data transfer between tables.

3. **`mytest.cpp`**
   - Test suite for validating the functionality and robustness of the `Cache` class.
   - **Tested Scenarios**:
     - Normal and edge cases for insertion, deletion, and retrieval.
     - Collision handling and rehashing during high load.
     - Constructor validation with invalid table sizes.
   - **Example Output**:
     - `INSERT NORMAL 1 PASSED`
     - `REMOVE REHASH 1 PASSED`
     - `CONSTRUCTOR ERROR 1 PASSED`

4. **`makefile`**
   - Automates the compilation of the project.
   - Includes instructions for building the `mytest` executable, linking it with `cache.cpp`.

---

## Compilation and Usage

1. **Build the Project**:
   - Run the following command to compile:
     ```bash
     make
     ```

2. **Run the Test Suite**:
   - Execute the test program:
     ```bash
     ./mytest
     ```

3. **Optional Debugging**:
   - Use `valgrind` to check for memory issues:
     ```bash
     valgrind ./mytest
     ```

---

## Sample Test Output

The test suite ensures the correctness of all functionalities. Sample output includes:

```plaintext
==2817107== Memcheck, a memory error detector
==2817107== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==2817107== Using Valgrind-3.23.0 and LibVEX; rerun with -h for copyright info
==2817107== Command: ./mytest
==2817107== 
INSERT NORMAL 1 PASSED
INSERT NORMAL 2 PASSED
INSERT ERROR PASSED
GET PERSON ERROR PASSED
GET PERSON NORMAL 1 PASSED
GET PERSON NORMAL 2 PASSED
REMOVE NORMAL 1 PASSED
REMOVE NORMAL 2 PASSED
INSERT REHASH NORMAL 1 PASSED
INSERT REHASH NORMAL 2 PASSED
INSERT REHASH NORMAL 3 PASSED
INSERT REHASH NORMAL 4 PASSED
INSERT REHASH NORMAL 5 PASSED
REMOVE REHASH 1 PASSED
REMOVE REHASH 2 PASSED
INSERT AND REMOVE 1 PASSED
INSERT AND REMOVE 2 PASSED
INSERT AND REMOVE 3 PASSED
INSERT AND REMOVE 4 PASSED
CONSTRUCTOR NORMAL 1 PASSED
CONSTRUCTOR NORMAL 2 PASSED
CONSTRUCTOR NORMAL 3 PASSED
CONSTRUCTOR ERROR 1 PASSED
CONSTRUCTOR ERROR 2 PASSED
CONSTRUCTOR ERROR 3 PASSED
==2817107== 
==2817107== HEAP SUMMARY:
==2817107==     in use at exit: 0 bytes in 0 blocks
==2817107==   total heap usage: 104 allocs, 104 frees, 8,784,544 bytes allocated
==2817107== 
==2817107== All heap blocks were freed -- no leaks are possible
==2817107==

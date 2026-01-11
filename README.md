# Data Structures and Simple Multi-Layer Perceptron (MLP) Framework

A C++ framework combining classical **data structures** and a **simple Multi-Layer Perceptron (MLP)** neural network for educational purposes.

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)  
[![xtensor](https://img.shields.io/badge/xtensor-0.24-orange.svg)](https://xtensor.readthedocs.io/)  
[![License](https://img.shields.io/badge/License-MIT%20%7C%20GPLv3-green.svg)](LICENSE)

## Overview

This project is a C++ framework developed as part of the **Data Structures and Algorithms course (CO2003) at Ho Chi Minh City University of Technology (VNU-HCM)**. It combines **data structures** and a **simple Multi-Layer Perceptron (MLP)** neural network, allowing students to implement, experiment, and extend classical data structures (lists, hash tables, heaps, graphs) and use them to build a basic neural network for classification tasks.

---

## Features

- Array-based and linked list implementations (`XArrayList`, `DLinkedList`).
- Hash table (`XHashMap`) and heap (`Heap`) data structures.
- Support for tensor operations using **xtensor** library.
- Simple Multi-Layer Perceptron (MLP) framework for classification tasks.
- Unit tests for all major data structures and neural network components.
- Demo programs for running MLP inference on sample datasets.

---

## Requirements

- **C++17** or higher
- **g++** (tested with g++ on Linux)
- **Make**
- **xtensor library** (included in the project)

Optional:

- Clang-format for code formatting

---

## Project Structure

```
.
├── Makefile               # Build instructions
├── main.cpp / main.hpp    # Main program entry
├── include/               # Header files
├── src/                   # Source files
├── models/                # Pre-trained model weights
├── datasets/              # Sample datasets
└── tests/                 # Unit test files
```

- `include/` contains all headers for data structures, neural network layers, tensor utilities, and helper functions.
- `src/` contains the implementation source files.
- `models/` stores pre-trained weights for MLP models.
- `datasets/` contains sample datasets for classification tasks.
- `tests/` contains unit test implementations for each module.

---

## Build & Run

### Build the main program

```bash
make program
./program
```

### Build & run unit tests

You can build and run unit tests for specific components:

```bash
# Lists
make xarraylist
make dlinkedlist

# Hash & Heap
make hash
make heap

# Neural Network layers
make layer_relu
make layer_sigmoid
make layer_tanh
make layer_softmax

# Loss function
make loss

# Graph & Topological sorting
make graph
make sort_topo
```

After building a test, run it using:

```bash
./main test_unit all         # Run all tests in the module
./main test_unit <TestName>  # Run a specific test function
```

### Clean build artifacts

```bash
make clean         # Remove binaries and object files
make clean_test    # Remove test binaries
```

### Code formatting

```bash
make format        # Format all .cpp/.hpp/.h files except sformat and tensor
```

### Notes

- `main` is used as the entry point for **unit tests**.
- `program` is the main executable to run **MLP inference demo**.

---

## Usage Example

For a simple MLP inference on a 2-class classification dataset:

```bash
make program
./program
```

The program will load the dataset from `datasets/2c-classification/` and the model weights from `models/2c-classification/`.

---

## License

This project is open-source. You can choose one of the following licenses:

- **MIT License:** Simple and permissive, allows reuse with minimal restrictions.
- **GPLv3:** Ensures derivative works remain open-source under the same license.

Choose the license that best fits your intended use of the project.

---

## References

- xtensor library for tensor operations: [https://xtensor.readthedocs.io](https://xtensor.readthedocs.io)
- Coursework instructions and assignments from **Data Structures and Algorithms (CO2003)** course.

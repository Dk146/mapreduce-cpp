# mapreduce-cpp

## Overview
This project is an attempt to build a MapReduce-like system in C++. The current implementation focuses on creating a multithreaded worker as the foundation for the system. Future iterations will expand on this by introducing inter-process communication and simulating random interruptions between worker and coorinator.

## Current Features
- **Multithreaded Worker**: The current implementation processes input files using multiple threads to simulate the "map" phase of MapReduce.

## Planned Improvement
1. **Inter-Process Communication**: Implement communication between multiple processes to simulate distributed systems.
2. **Random Interruptions**: Add functionality to simulate random interruptions in the connections between processes.
3. **Additional Tests**: Expand the test suite to cover edge cases and ensure robustness.

## Build 
mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..

make

./worker ../src/input_file/*.txt

### or just simply run:
bash wordcount-test.sh

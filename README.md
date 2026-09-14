# Coded Caching Simulator

C++ simulator used for the thesis:

**Community-Aware Cache Placement With Heterogeneous User Demand**

This repository contains the coded-caching simulation framework used to evaluate caching and coded delivery under user content requests.

## Project structure

- `testTxRx-BER_paper/config.h`  
  Main simulation configuration, including the number of users, number of files, cache size, input paths, and simulation run ID.

- `testTxRx-BER_paper/create_request.cpp`  
  Generates user content requests.

- `testTxRx-BER_paper/environment/`  
  Reads the input probabilities and requests, and creates the simulation environment.

- `testTxRx-BER_paper/cache/`  
  Performs cache placement by assigning file chunks to user caches.

- `testTxRx-BER_paper/testTxRx/`  
  Builds the conflict graph and evaluates coded delivery.

- `testTxRx-BER_paper/Shared Library/`  
  Contains the C++ library used for conflict-graph construction and graph coloring.

## Input data

The simulator uses CSV files containing user probabilities and request realizations. The input paths are set in `config.h`.

The LightGCN preference-learning model, Louvain community detection, and data-preparation scripts are available at:

https://github.com/rim-abdallah/LightGCN_MovieLens_Thesis

## Running the simulator

Set the required paths and parameters in `testTxRx-BER_paper/config.h`.

Compile and run the simulation components according to the scripts and makefiles included in the project folders.

## Notes

Generated XML files, executable files, and large input/output CSV files are excluded from this repository.

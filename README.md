# Solitaire
## About
This repository contains the research code used to create test algorithms and their ability to solve 1-Draw Klondike solitaire without back tracking and with hidden information. Two methods are explored Monte-Carlo & Reinforcement learning. Both methods currently working progress and written in C++.

## Usage
Each branch contains a CMake File as well as the MainProjectCode folder this is where all the Code is kept. CMake is needed to compile the code.
Before running it is advised that the resource file path is updated in the CMake file for any classes that involve file output. 
### C++Version Branch
In the C++Version Branch is the core Solitaire environment with Greedy and Random AI players. 

### Monte Carlo
The Monte Carlo Branch contains the code for a MCTS on the environment the end result is outputted to a text file. This code how a long run time and is multi thread to 10 threads. If you wish to run it on less threads the thread count will need to be change in the main.cpp file. Be aware that the default run of 100 simulated games using MCTS has a run time of ~+120mins based on system capabilities.

### Sisyphus and Reinforcement learning
Sisyphus and Reinforcement learning branches are working progress and should not be run.

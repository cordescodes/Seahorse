# Seahorse Chess Engine

### Overview
A chess engine implemented in C++ using Minimax Search with Alpha-Beta Pruning. Can be linked to a chess GUI 
(such as Arena) which uses the UCI protocol.

Seahorse currently searches to a depth of 5 and only uses piece values for evaluation. 
Seahorse does not support searching to a specific depth or the use of time controls.

### Usage
A 32-bit Windows executable is available in the `bin\Debug` folder. Seahorse is built in the MinGW environment and requires the presence of the libstdc++-6.dll package (also found in the `bin\Debug` folder) in the same directory as the executable.

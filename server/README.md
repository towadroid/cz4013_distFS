# Server application

## Build from source
The application can be built form source by executing the following commands:
```bash
cd server
mkdir build && cd build
cmake ..
cmake --build ./ --target server
```

### Dependencies
* Git
* CMake ≥ 3.11
* GCC ≥ 8.1
* Internet connection

## Usage
The server can then be run with `./server [-am | -al] [-f <FAILURE RATE>] [-p <PORT>] [-v [<LOG LEVEL>]] [-s <SEED>]`.
Use `./server --help` to display further explanation.

To run the tests additionally execute the following commands:
```bash
cmake --build ./ --target tests
./tests
```
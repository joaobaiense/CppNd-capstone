
# Capstone Description 
This project is a capstone for the Udacity C++ Nanodegree. The option chosen was "Capstone Option 1." This project is a stock portfolio simulator that loads stock data and perform two algorithms to compare the portfolio performance after iterating through all the available data. After processing the data, the program reports each portfolio's performance relative to the starting balance. 

The porfolios use two algorithms for randomly placing purchase/sell on available stock based on the current balance. `PortfolioRandom` implements a random purchase/sell algorithm, whereas `PortfolioRandom2` performs the same algorithm as well as offloading any stocks which fall below the purchase value. 

## Rubric Overview

### Compiling and Testing 
| Result             | Criteria                             | Notes                          |
| ------------------ | :----------------------------------- | ------------------------------ |
| :white_check_mark: | The submission must compile and run. | The project compiles and runs. |

### Loops, Functions, I/O
| Result             | Criteria                                                                                       | Notes                                                                                                     |
| ------------------ | :--------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------- |
| :white_check_mark: | The project demonstrates an understanding of C++ functions and control structures.             |                                                                                                           |
| :white_check_mark: | The project reads data from a file and process the data, or the program writes data to a file. | src/main.cpp:109 Read in CSV file.                                                                        |
| :white_check_mark: | The project accepts user input and processes the input.                                        | src/main.cpp:89 The user can pass in a command line argument specifying the portfolio's starting balance. |

### Object Oriented Programming
| Result             | Criteria                                                                         | Notes                                      |
| ------------------ | :------------------------------------------------------------------------------- | ------------------------------------------ |
| :white_check_mark: | The project uses Object Oriented Programming techniques.                         | types/include/Types/Portfolio.hpp          |
| :white_check_mark: | Classes use appropriate access specifiers for class members.                     | types/include/Types/Portfolio.hpp:61       |
| :white_check_mark: | Class constructors utilize member initialization lists.                          | types/include/Types/Portfolio.hpp:42       |
| :white_check_mark: | Classes abstract implementation details from their interfaces.                   |                                            |
| :white_check_mark: | Classes encapsulate behavior.                                                    |                                            |
| :white_check_mark: | Classes follow an appropriate inheritance hierarchy.                             |                                            |
| :white_check_mark: | Overloaded functions allow the same function to operate on different parameters. | types/include/Types/Portfolio.hpp:51       |
| :white_check_mark: | Derived class functions override virtual base class functions.                   | types/include/Types/PortfolioRandom2.hpp:8 |
| :white_check_mark: | Templates generalize functions in the project.                                   | utils/include/Utils/Queue.hpp:9            |

### Memory Management
| Result             | Criteria                                                                                  | Notes                            |
| ------------------ | :---------------------------------------------------------------------------------------- | -------------------------------- |
| :white_check_mark: | The project makes use of references in function declarations.                             |                                  |
| :white_check_mark: | The project uses destructors appropriately.                                               |                                  |
| :white_check_mark: | The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate. |                                  |
| :white_check_mark: | The project follows the Rule of 5.                                                        |                                  |
| :white_check_mark: | The project uses move semantics to move data, instead of copying it, where possible.      | utils/include/Utils/Queue.hpp:24 |
| :white_check_mark: | The project uses smart pointers instead of raw pointers.                                  | src/main.cpp:111                 |

### Concurrency 
| Result                | Criteria                                     | Notes                                                   |
| --------------------- | :------------------------------------------- | ------------------------------------------------------- |
| :white_check_mark:    | The project uses multithreading.             | src/main.cpp:167                                        |
| :black_square_button: | A promise and future is used in the project. | :warning: This project does not employ promises/futures |
| :white_check_mark:    | A mutex or lock is used in the project.      | utils/include/Utils/Queue.hpp:16                        |
| :white_check_mark:    | A condition variable is used in the project. | utils/include/Utils/Queue.hpp:19                        |


# Build/Run Dependencies 

This project depends on `CMake`, `google tests`, and `spdlog`. All dependencies, except for `CMake` are included in the vendor directory.


# Build Project 

To compile this project, run the following commands:
```
mkdir build && cd build 
cmake -DCMAKE_BUILD_TYPE=Release .. 
```

# Run project 

From the root of the project, run `./build/bin/stockSim` to run the default program with a balance of $ 10,000. To specify a different starting balance, provide the balance as a command line parameter, `./build/bin/stockSim 5000`.

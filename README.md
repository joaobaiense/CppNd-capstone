
# Capstone Description 

## Rubric Overview


### Compiling and Testing 
| Result             | Criteria                             | Notes |
| ------------------ | :----------------------------------- | ----- |
| :white_check_mark: | The submission must compile and run. |       |

### Loops, Functions, I/O
| Result             | Criteria                                                                                       | Notes |
| ------------------ | :--------------------------------------------------------------------------------------------- | ----- |
| :white_check_mark: | The project demonstrates an understanding of C++ functions and control structures.             |       |
| :white_check_mark: | The project reads data from a file and process the data, or the program writes data to a file. |       |
| :white_check_mark: | The project accepts user input and processes the input.                                        |       |

### Object Oriented Programming
| Result                | Criteria                                                                         | Notes                                                  |
| --------------------- | :------------------------------------------------------------------------------- | ------------------------------------------------------ |
| :white_check_mark:    | The project uses Object Oriented Programming techniques.                         |                                                        |
| :white_check_mark:    | Classes use appropriate access specifiers for class members.                     |                                                        |
| :white_check_mark:    | Class constructors utilize member initialization lists.                          |                                                        |
| :white_check_mark:    | Classes abstract implementation details from their interfaces.                   |                                                        |
| :white_check_mark:    | Classes encapsulate behavior.                                                    |                                                        |
| :black_square_button: | Classes follow an appropriate inheritance hierarchy.                             | :warning: This project does not use class inheritance. |
| :black_square_button: | Overloaded functions allow the same function to operate on different parameters. | :warning: This project does not use class inheritance. |
| :black_square_button: | Derived class functions override virtual base class functions.                   | :warning: This project does not use class inheritance. |
| :white_check_mark:    | Templates generalize functions in the project.                                   |                                                        |

### Memory Management
| Result             | Criteria                                                                                  | Notes |
| ------------------ | :---------------------------------------------------------------------------------------- | ----- |
| :white_check_mark: | The project makes use of references in function declarations.                             |       |
| :white_check_mark: | The project uses destructors appropriately.                                               |       |
| :white_check_mark: | The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate. |       |
| :white_check_mark: | The project follows the Rule of 5.                                                        |       |
| :white_check_mark: | The project uses move semantics to move data, instead of copying it, where possible.      |       |
| :white_check_mark: | The project uses smart pointers instead of raw pointers.                                  |       |

### Concurrency 
| Result                | Criteria                                     | Notes                                                   |
| --------------------- | :------------------------------------------- | ------------------------------------------------------- |
| :white_check_mark:    | The project uses multithreading.             |                                                         |
| :black_square_button: | A promise and future is used in the project. | :warning: This project does not employ promises/futures |
| :white_check_mark:    | A mutex or lock is used in the project.      |                                                         |
| :white_check_mark:    | A condition variable is used in the project. |                                                         |


# Project Description 

# Build/Run Dependencies 

Cmake 
** ?? 

spdlog, google tests

# Build Project 

```
mkdir build && cd build 
cmake -DCMAKE_BUILD_TYPE=Release .. 
```

# Run project 

```
build/bin/stockStim
build/bin/stockSim <portfolio_balance=10000>
```

# Random Buy/Sell Signal Algorithm

# Simulator Performance

?? How well does the random simulator 
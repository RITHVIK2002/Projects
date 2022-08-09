# Report

## Introduction

This language is designed for those work on the matrices integrated topics. ***maTricks main focus on expressiveness and performance of matrix operations***. We will make most of the matrix operations built in. 

### [Tutorial](./Tutorial.md) 
### [Reference manual](./4_langugage_specification.pdf)
### [Project Plan](https://docs.google.com/spreadsheets/d/1GYYlhVclNoURhhQTDrZ4zn8Ibiq6_UsrSNRQLt0ZmB0/)

## Language evolution

The language is evolution is straight forward, with the basic features in mind we were able to finalise many things before start of work. But semantics we assumed are hard to implement and some semantics are not well framed earlier (like scope of functions, structs etc) are corrected later while working on respective features. 

## Compiler architecture

There is a "main.c" responsible for the cli behaviour and cooridnates the pass of information between different phases based on the inputs provided in cli. 
Next all the code related to each phase are separated into their respective directories.
* include - Contains all the header files used by all the files
* lexer - lexcial specification 
* parser - All the Grammar and Semantics (as it they closely related) and symbol table implemntations
* codegen - IR generation and machine code generation

The intercommunication between differnt files is achieved using *extern's* and header files

## Development environment 

All the development and testing is done on the x86_64 machine running Ubuntu 21.04. To build this the dependices need are
* Flex 
* GNU Bison 
* gcc and g++
* libeigen3-dev 
    * It is also a run-time dependency 
* GNU Make

All the build is automated by using a *Makefile*. To trace the header dependices of source files (including the generated lex and bison 'C' files) the features of GNU Make and its two-phase pass mechanism are used. All the object files and other binaries reside in the *build* folder and the final executable *maTricks* is generated in the root folder.


## Test plan and test suites 

Different Test suites are written by the testers to test particular aspects of the compiler when working with the corresponding phase/feature. All the test snippets are in the examples directory 

## Conclusions and lessons learned 

This project taught us lots of things in the both academic and mangaing perspectives. It acted as a hands-on to implement the theory learnt in the course in our lanugage (in different phases). This taught us how to coordinate and distribute the workload and also the many software engineering principles (How to organize the code, develop the devoplement/test environment for others to use, develop nesscary infrastructure etc..) Also It taught us many aspects in the C programming language (in the view of compiler) and also the many things about build system, dependency managment, and version control handling.

## Appendix (source listing)
* [Kalideoscope](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html)
* [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
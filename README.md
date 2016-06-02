# What is superoptimizer?

## In theory

## In pratical

We choose souper as superoptimizer because of its documentation and example. This project is made by some people from Google. Souper need a SMT solver to prove equivalence between some LLVM IR found in a program and a smaller 

# Setup

## Requirement

**LLVM**

Because this project is about LLVM.

[Get LLVM](http://llvm.org/)


**souper**

Souper is a superoptimizer for LLVM IR. It uses an SMT solver to help identify missing peephole optimizations in LLVM's midend optimizers.

This repository is included as a submodule in this project. You just have to git submodule init / git submodule update ;)

```git clone https://github.com/google/souper.git```


**svn**

Needed by souper to build and update dependencies.

[Get SVN](https://subversion.apache.org/packages.html)


**cmake**

Needed by souper.

[Get cmake](https://cmake.org/download/)


**CVC4**

Souper need a *SMT solver* to work. You have the choice between Boolector, CVC4, STP or Z3. We choose CVC4 because of its good documentation and examples.

[Get CVC4](http://cvc4.cs.nyu.edu/downloads/)

## Building Souper

You can find all these steps in [souper repository](https://github.com/google/souper/blob/master/README). Steps below are just simplified.

First, you have to build and update all the dependencies in ```code/souper``` directory. It takes a long time...

```./update_deps.sh $buildtype $extra_cmake_flags```

After that, you can build souper. In ```code/souper``` directory, do:

- $ mkdir souper-build
- $ cd souper-build
- $ cmake -DCMAKE_BUILD_TYPE=$buildtype ../
- $ cd souper-build
- $ make

You can optionally run ```make check``` to run Souper's test suite.

# Using Souper

You can find all these steps in [souper repository](https://github.com/google/souper/blob/master/README). Steps below are just simplified.


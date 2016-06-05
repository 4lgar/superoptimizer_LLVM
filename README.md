# Superoptimizers - Souper and LLVM

## Introduction
Hardware is not becomming faster but stranger. Since the physical limitations don't allow us to go faster we have to find other ways to do more with the hardware. This gave birth to many heterogenous architectures (CPUs, GPUs, FPGAs, Acceleration cards etc...). At the same time the projects have become much higher level, much bigger and more complex. We want our projects to go as fast as possible and without having to know every detail about all thoses specific hardware architectures. To do so we have compilers !
We want our compilers to be :
- Fast : We don't want to wait for our code to compile, for example if we use real-time compilation to highlight errors in our code (ex : Errors shown in Eclipse) we want it to be fast.
- Effective : The compiled code should be as fast as possible.
- Correct : The code should do what it is supposed to do ! (Research showed bugs in many compilers, GCC, LLVM and even CompCert which is proven to be correct ! (The bugs where in the front and backend of CompCert) so no compiler totally bug safe).

We will mainly talk about Effectiveness and Correctness. So are the compilers good at making code faster ?
It depends. Optimization is implemented in three main steps :

1. Analysis : Is the code optimizable.
2. Safety check : Is the optimization safe?
3. Transformation : Adapt the code with the optimization.

This will be done in multiple passes.

But adding optimizations to the compiler is costly, both in compile time and in development time (and maintenance). 
Why ? Because we are human beings.
- The analysis is complicated, we have to be sure to analyse eveything, every corner case.
- If we make the safety check too permissive it will allow bug prone optimizations.
- Our transformations could be wrong.

So this gave birth to an Idea : Discover optimizations automatically.
These optimization have the advantage not to be hand-coded so they are less expensive and less bug-prone).

The term Superoptimizer appeared in 1987 (Massalin 87) who bruteforced combinations of machine code to get the smallest machine code for a given task. There were just a few tests (so this led us to beleive that the optimizations were correct and sometimes they weren't).
So here the Effectiveness of the compiler was improved but not the correctness.

So there was a problem of having correct optimizations and to solve this problem one way was adding an automated theorem prover. That is we transform every instruction into a predicates the combination of these predicates give us a specific behaviour (or theorem). We can then check if the instructions of the optimized code then also verify this given theorem. If so the optimization is proved to be correct. (This could not be done with bruteforce because the input space is way to big).

The idea of a modern Superoptimizer is to find optimizations that are totally correct. We have two main ways to use superoptimizers :

1. Find optimizations during compile time for making faster programs.
2. Find general optimizations to make compilers better at making fast programs.

An example is by Sands (2011) making a Super-Optimizer for LLVM IR who found many optimizations that were missing in LLVM. (a lot of very simple optimizations).

An other example is the Souper project. Souper tried to address both goals. A lot of work has been done searching for optimizations will building LLVM itself with LLVM Super-Optimized by Souper.

So what Souper does is :

1. It takes the LLVM IR (Intermediate Representation) bytecode and converts it in its own IR (basically a simplified LLVM IR).
2. And turns these instructions into a DAG (Directed Acyclic Graph). This has many advantages, it shows the dependencies but is also easy to (de)serialize by simple graph traversal.
3. Then it turns the instructions into predicates (in SMT-Lib format, Satisfiability Modulo Theories).
4. This will be used in a SMT solver in order to prove the optimizations correctness.

Souper works on the DAGs and searches for very simple optimizations like replacing instructions with a constant or a value that is already found somewhere. These are bery basic but the developers knew they wouldn't make things worse and are easy to prove. This gave them good results already. Once Souper could replace an instruction the bytecode was then passed again into LLVM, LLVM would then remove all the dead code (unused code) and give a new IR result. This would then go to Souper again to try finding optimizations.

Souper also checks for code that can never be reached due to branch conditions, these appear in the low level code (IR or assembly) and are often not optimized by compilers (GCC or LLVM) because of function calls or inlining. Since the compilers optimize at function level mainly it is really possible to have code where conditions are checked twice or will never be true. Since Souper works at a lower level it will detect these fairly easily. It can then removed the never reached code or combine the branch conditions.

Another way Souper optimizes code is by using undefined behaviour (UB). UB may appaear in C or C++ but also when another language is used that has no UB, like lisp or haskell. Why ? this is because LLVM IR has instructions that have UB. (because they are partial functions and are only defined for a subset of the input space). So the super-optimizer makes the assumption that the code is not relying on UB and can use this gray space for making more optimizations. Since UB is not bound to any particular behaviour the optimizer can change it to do optimizations better !

So the super-optimizer and the compiler work together to make faster programs but the optimizations found could also be implemented in the compiler itself, some are some are not. So to speed up the compilation many optimization found while building programs are cached into a database. This would allow to directly implement them if a similar piece of code is found (not only the cases with the same instructions but in general the cases that when transformed into predicates prove the same theorem).

## Conclusion
This approach benefits everyone, from the compiler users to the compiler developpers (and then also the compiler users again). The questions of which optimizations to implement in a real compiler could arise. This like any other question around optimization requires profiling. There are two main ways to investigate this :

1. Find the optimizations that are the most often used during compile time. (this would make compile time faster but only makes a faster compiler).
2. Find which optimization's code is the most used at run time for the compiled program. (Of course this shoudl be tested with a wide range of programs, for example a linux kernel could give us a good idea) This would make a more efficient compiler.

A lot of other things could be analysed for making a better super-optimizer like exploit dataflow facts (LLVM knows a lot about the data). But there is already a lot to learn from Souper. Why would we investigate Souper ? Because it is open-source and because it already made LLVM a better compiler !

## Sources and references

## Setup

### Requirement

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

CVC4 is included in ```code/cvc```

[Get CVC4](http://cvc4.cs.nyu.edu/downloads/)

### Building Souper

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

## Using Souper

You can find all these steps in [souper repository](https://github.com/google/souper/blob/master/README). Steps below are just simplified.

To simply compile a c file, use the following command:

```
path/to/souper/third_party/llvm/Release/bin/clang -g -O3 -Xclang -load -Xclang path/to/souper/souper-build/libsouperPass.so -mllvm -cvc4-path=path/to/cvc/cvc4-1.4-x86_64-linux-opt yourFileToCompile.c
```

## Examples

### Simple test

in ```code/test/simple/main.c```, we have two simple functions:

```
uint32_t foo(uint32_t a, uint32_t b){
	return a*8 + b;
}

uint32_t bar(uint32_t a, uint32_t b){
	uint32_t c = a * b;

	if(a < b)
		if(b < c)
			if(a > c)
				return foo(a, b);

	return 0;
}
```

```foo``` simply calculate some output and ```bar``` always return 0 because of impossible condition. We hope that the optimizer will find a simpler way to compute ```foo``` output and remove useless condition ```bar```.

Here is the generated asm with clang -O3:

```
0000000000400520 <foo>:
  400520:	8d 04 fe             	lea    (%rsi,%rdi,8),%eax
  400523:	c3                   	retq   
  400524:	66 66 66 2e 0f 1f 84 	data16 data16 nopw %cs:0x0(%rax,%rax,1)
  40052b:	00 00 00 00 00 

0000000000400530 <bar>:
  400530:	89 f1                	mov    %esi,%ecx
  400532:	0f af cf             	imul   %edi,%ecx
  400535:	31 c0                	xor    %eax,%eax
  400537:	39 f9                	cmp    %edi,%ecx
  400539:	73 0b                	jae    400546 <bar+0x16>
  40053b:	39 f7                	cmp    %esi,%edi
  40053d:	73 07                	jae    400546 <bar+0x16>
  40053f:	39 f1                	cmp    %esi,%ecx
  400541:	76 03                	jbe    400546 <bar+0x16>
  400543:	8d 04 fe             	lea    (%rsi,%rdi,8),%eax
  400546:	c3                   	retq   
  400547:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  40054e:	00 00 
```

Here is the generated asm with souper

```
0000000000400520 <foo>:
  400520:	8d 04 fe             	lea    (%rsi,%rdi,8),%eax
  400523:	c3                   	retq   
  400524:	66 66 66 2e 0f 1f 84 	data16 data16 nopw %cs:0x0(%rax,%rax,1)
  40052b:	00 00 00 00 00 

0000000000400530 <bar>:
  400530:	31 c0                	xor    %eax,%eax
  400532:	c3                   	retq   
  400533:	66 66 66 66 2e 0f 1f 	data16 data16 data16 nopw %cs:0x0(%rax,%rax,1)
  40053a:	84 00 00 00 00 00 
```

As we can see, both clang -O3 and souper found that parameters of ```foo``` are constant in our programm and replace the result by a constant. But clang -O3 compile the 3 conditionals tests for ```bar```, a missed optimization that souper found!
Souper win 500k with these optimizations.

### Bubble sort

With souper, we won 4 seconds sorting 100000 elements (17s vs 13s). The size are pretty similar.

We tried to sort less element (10000). In this case, souper will be more longer than clang -O3 version (0.136s vs 0.143s). In addition, souper binary will be the same size. That's normal because of constant replacement.

### Matrix inverter

Souper doesn't do it well: Both size (27k vs 15k) and time (2.2s vs 1.9s) are affected.

That's totally normal because souper does not optimize floating point instruction. We tried anyway to compile our matrix inverter lab that we did in HPC to show what's going on.

### Conclusion

Gains are not really significative in these examples. Because it found little optimizations, souper will work better on larger code base. John Regehr told us that souper compile a clang that was 3Mb smaller than the version compiled with -O3 version.

Souper is under construction. But at this point, it can actually make the diffrence, even if it can only optimize i1 values that it can prove to be 0 or 1. 
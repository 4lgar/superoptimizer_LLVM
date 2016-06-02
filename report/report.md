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

An other example is the Souper project by Google. Souper tried to address both goals. A lot of work has been done searching for optimizations will building LLVM itself with LLVM Super-Optimized by Souper.

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

## Examples

## Sources and references
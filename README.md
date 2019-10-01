## Introduction

The second assignment has two purposes is really where we start doing Computer Graphics. The main focus will be on pushing the ideas we explored in 1D (the previous assignment) to larger scale examples (bunnies!) in 3D. 

### Prerequisite installation

On all platforms, we will assume you have installed cmake and a modern c++
compiler on Mac OS X[¹](#¹macusers), Linux[²](#²linuxusers), or
Windows[³](#³windowsusers).

We also assume that you have cloned this repository using the `--recursive`
flag (if not then issue `git submodule update --init --recursive`). 

**Note:** We only officially support these assignments on Ubuntu Linux 18.04 (the OS the teaching labs are running) and OSX 10.13 (the OS I use on my personal laptop). While they *should* work on other operating systems, we make no guarantees. 

**All grading of assignments is done on Linux 18.04**

### Layout

All assignments will have a similar directory and file layout:

    README.md
    CMakeLists.txt
    main.cpp
    assignment_setup.h
    include/
      function1.h
      function2.h
      ...
    src/
      function1.cpp
      function2.cpp
      ...
    data/
      ...
    ...

The `README.md` file will describe the background, contents and tasks of the
assignment.

The `CMakeLists.txt` file setups up the cmake build routine for this
assignment.

The `main.cpp` file will include the headers in the `include/` directory and
link to the functions compiled in the `src/` directory. This file contains the
`main` function that is executed when the program is run from the command line.

The `include/` directory contains one file for each function that you will
implement as part of the assignment.

The `src/` directory contains _empty implementations_ of the functions
specified in the `include/` directory. This is where you will implement the
parts of the assignment.

The `data/` directory contains _sample_ input data for your program. Keep in
mind you should create your own test data to verify your program as you write
it. It is not necessarily sufficient that your program _only_ works on the given
sample data.

## Compilation for Debugging

This and all following assignments will follow a typical cmake/make build
routine. Starting in this directory, issue:

    mkdir build
    cd build
    cmake ..

If you are using Mac or Linux, then issue:

    make

## Compilation for Testing

Compiling the code in the above manner will yield working, but very slow executables. To run the code at full speed, you should compile it in release mode. Starting in the **build directory**, do the following:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    
Followed by:

    make 
  
Your code should now run significantly (sometimes as much as ten times) faster. 

If you are using Windows, then running `cmake ..` should have created a Visual Studio solution file
called `a3-finite-elements-3d.sln` that you can open and build from there. Building the project will generate an .exe file.

Why don't you try this right now?

## Execution

Once built, you can execute the assignment from inside the `build/` using 

    ./a3-finite-elements-3d

## Background

In this assignment you will get a chance to implement one of the  gold-standard methods for simulating elastic objects -- the finite element method (FEM). Unlike the particles in the previous [assignment](https://github.com/dilevin/CSC2549-a2-mass-spring-3d), the finite-element method allows us compute the motion of continuous volumes of material. This is enabled by assuming that the motion of a small region can be well approximated by a simple function space. Using this assumption we will see how to generate and solve the equations of motion.   

FEM has wonderfully practical origins, it was created by engineers to study [complex aerodynamical and elastic problems](https://en.wikipedia.org/wiki/Finite_element_method) in the 1940s. My MSc supervisor used to regale me with stories of solving finite element equations by hand on a chalkboard. With the advent of modern computers, its use as skyrocketed.  

FEM has two main advantages over mass-spring systems. First, the behaviour of the simulated object is less dependent on the topology of the simulation mesh. Second, unlike the single stiffness parameter afforded by mass spring systems, FEM allows us to use a richer class of material models that better represent real-world materials. 

## Resources

Part I of this [SIGGRAPH Course](http://www.femdefo.org), by Eftychios Sifakis and Jernej Barbic, is an excellent source of additional information and insight, beyond what you will find below. 

## Our Geometric Primitive: The Tetrahedron

## A Piecewise-Linear Function Space 

## The Generalized Coordinates 

## The Kinetic Energy of a Single Tetrahedron

## The Deformation of a Single Tetrahedron

## The Potential Energy of a Single Tetrahedron

### The Strain Energy density

### Numerical quadrature

## Forces and stiffness

## From a Single Tetrahedron to a Mesh 

## Time integration

### Linearly-Implicit Euler

### Backward (Implicit) Euler

### Newton's method

### Line Search

## Assignment Implementation 

### Implementation Notes

For this course most functions will be implemented in **.cpp** files. In this assignment the only exception is that time integrators are implemented in **.h** files. This is due to the use of lambda functions to pass force data to the time integration algorithms. Finite element derivatives are both tricky and tedious to do correctly. Because of this you **DO NOT** have to take these derivatives by hand (unless you want to show off your mad skills). You can use a symbolic math package such as *Maple*, *Mathematica* or *Matlab*. **Note:** You can not use automatic differentiation, only symbolic math packages.

### Important

Some of the functions from assignment 2 are reused here. If you correct implementation errors in those functions, your grades for the previous assignment will be updated to reflect that. 

### phi_linear_tetrahedron.cpp  

Evaluate the linear shape functions for a tetrahedron. This function returns a 4D vector which contains the values of the shape functions for each vertex at the world space point x (assumed to be inside the element).  

### dphi_linear_tetrahedron_dX.cpp  

Piecewise constant gradient matrix for linear shape functions. Row $i$ of the returned matrix contains the gradient of the $i^{th}$ shape function.  

### psi_neo_hookean.cpp  

Compute the Neo-Hookean strain energy density.

### dpsi_neo_hookean_dF.cpp  

Compute the first Piola-Kirchoff (the gradient of the strain energy density with respect to the deformation gradient). You can use a symbolic math package to do this (but don't just look it up on the internet please).

### d2psi_neo_hookean_dF2.cpp 

Compute the hessian of the strain energy density with respect  to the deformation gradient. You can use a symbolic math package to do this (but don't just look it up on the internet please).  

### T_linear_tetrahedron.cpp 

Compute the kinetic energy of a single tetrahedron.  

### quadrature_single_point.h

Single point quadrature for a constant strain tetrahedron (CST).  

### V_linear_tetrahedron.cpp  

Compute the potential energy of a single tetrahedron. **Note:** you will need both *psi_neo_hookean.cpp* and *quadrature_single_point.h* to do this.  

### dV_linear_tetrahedron_dq.cpp  

Compute the gradient of the potential energy of a single tetrahedron. **Note:** you will need both *dpsi_neo_hookean_dq.cpp* and *quadrature_single_point.h* to do this.  

### d2V_linear_tetrahedron_dq2.cpp  

Compute the hessian of the potential energy of a single tetrahedron. **Note:** you will need both *d2psi_neo_hookean_dq2.cpp* and *quadrature_single_point.h* to do this.

### V_spring_particle_particle.cpp  

The potential energy of a non-zero rest length spring attached to two vertices of the mesh. **Use your code from the last assignment**.  

### dV_spring_particle_particle_dq.cpp  

The gradient of the spring potential energy. **Use your code from the last assignment**.  

### mass_matrix_linear_tetrahedron.cpp  

Compute the dense mass matrix for a single tetrahedron.  

### mass_matrix_mesh.cpp  

Assemble the full mass matrix for the entire tetrahedral mesh.   

### assemble_forces.cpp  

Assemble the global force vector for the finite element mesh.  

### assemble_stiffness.cpp  

Assemble the global stiffness matrix for the finite element mesh.   

### build_skinning_matrix.cpp  

Build the skinning matrix that maps position from the coarse simulation mesh to the high resolution rendering mesh. 

### fixed_point_constraints.cpp  

**Use your code from the last assignment**

### pick_nearest_vertices.cpp  

**Use your code from the last assignment**

### linearly_implicit_euler.h  

**Use your code from the last assignment**

### newtons_method.h   

Implement Newton's method with backtracking line search. Use the following parameter values: *alpha (initial step length) = 1*, *p (scaling factor) = 0.5*, *c (ensure sufficient decrease) = 1e-8*. 

### implicit_euler.h  

Using your Newton's method, implement a fully implicit solver. **To ensure reasonable performance, use a maximum of five (5) iterations**.



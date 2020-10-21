## Introduction

The third assignment will introduce you to the most common numerical method for simulating, well, almost anything in Computer Graphics, the mighty Finite Element Method.  

### Prerequisite installation

On all the platforms, we will assume you have installed cmake and a modern c++
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

In this assignment you will get a chance to implement one of the  gold-standard methods for simulating elastic objects -- the finite element method (FEM). Unlike the particles in the previous [assignment](https://github.com/dilevin/CSC417-a2-mass-spring-3d), the finite-element method allows us compute the motion of continuous volumes of material. This is enabled by assuming that the motion of a small region can be well approximated by a simple function space. Using this assumption we will see how to generate and solve the equations of motion.   

FEM has wonderfully practical origins, it was created by engineers to study [complex aerodynamical and elastic problems](https://en.wikipedia.org/wiki/Finite_element_method) in the 1940s. My MSc supervisor used to regale me with stories of solving finite element equations by hand on a chalkboard. With the advent of modern computers, its use as skyrocketed.  

FEM has two main advantages over mass-spring systems. First, the behaviour of the simulated object is less dependent on the topology of the simulation mesh. Second, unlike the single stiffness parameter afforded by mass spring systems, FEM allows us to use a richer class of material models that better represent real-world materials. 

## Resources

Part I of this [SIGGRAPH Course](http://www.femdefo.org), by Eftychios Sifakis and Jernej Barbic, is an excellent source of additional information and insight, beyond what you will find below. 

![Armadillo simulated via Finite Element Elasticity](images/armadillo.gif)

## The Finite Element method

The idea of the finite element method is to represent quantities inside a volume of space using a set of scalar *basis* or *shape* functions <img src="images/ec7986eb29346b1eae6b36942b0fb975.svg?invert_in_darkmode" align=middle width=40.76954639999999pt height=24.65753399999998pt/> where <img src="images/14b88c29f249c16ee64da346bc37d777.svg?invert_in_darkmode" align=middle width=50.55236834999999pt height=26.76175259999998pt/> is a point inside the space volume. We then represent any quantity inside the volume as a linear combination of these basis functions:

<p align="center"><img src="images/fe6804f6bc753bbf4631ab6c9542bec5.svg?invert_in_darkmode" align=middle width=141.73209435pt height=47.93392394999999pt/></p>

where <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/> are weighting coefficients. Designing a finite element method involves making a judicious choice of basis functions such that we can compute the <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/>'s efficiently. Spoiler Alert: in the case of elastodynamics, these <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/>'s will become our generalized coordinates and will be computed via time integration. 

## Our Geometric Primitive: The Tetrahedron

For this assignment we will use a [tetrahedron](https://en.wikipedia.org/wiki/Tetrahedron) as the basic space volume. The reason we work with tetrahedra is two-fold. First, as you will see very soon, they allow us to easily define a simple function space over the volume. Second, there is available [software](https://github.com/Yixin-Hu/TetWild) to convert arbitrary triangle meshes into tetrahedral meshes.

![A Tetrahedron](https://upload.wikimedia.org/wikipedia/commons/8/83/Tetrahedron.jpg)  

## A Piecewise-Linear Function Space 

Now we are getting down to the nitty-[gritty](https://en.wikipedia.org/wiki/Gritty_(mascot)) -- we are going to define our basis functions. The simplest, useful basis functions we can choose are linear basis functions, so our goal is to define linear functions inside of a tetrahedron. Fortunately such nice basis functions already exist! They are the [barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system). For a tetrahedron there are four (4) barycentric coordinates, one associated with each vertex. We will choose <img src="images/c83e439282bef5aadf55a91521506c1a.svg?invert_in_darkmode" align=middle width=14.44544309999999pt height=22.831056599999986pt/> to be the <img src="images/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode" align=middle width=18.32504519999999pt height=27.91243950000002pt/> barycentric coordinate. 

Aside from being linear, barycentric coordinates have another desireable property, called the *[Kronecker delta](https://en.wikipedia.org/wiki/Kronecker_delta) property* (or fancy Identity matrix as I like to think of it). This is a fancy-pants way of saying that the <img src="images/c36ef0ba4721f49285945f33a25e7a45.svg?invert_in_darkmode" align=middle width=20.92202969999999pt height=26.085962100000025pt/> barycentric coordinate is zero (0) when evaluated at any vertex, <img src="images/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/>, of the tetrahedron, <img src="images/6e355470c45cc13b0430c284142cf243.svg?invert_in_darkmode" align=middle width=35.29127414999999pt height=22.831056599999986pt/>, and one (1) when evaluated at <img src="images/33bf811999b31b361dfb87ee222620f3.svg?invert_in_darkmode" align=middle width=35.29127414999999pt height=21.68300969999999pt/>. What's the practical implication of this? Well it means that if I knew my function <img src="images/60c7f8444e2f683eb807f4085729add6.svg?invert_in_darkmode" align=middle width=34.73748959999999pt height=24.65753399999998pt/>, then the best values for my <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/>'s would be <img src="images/81f710835c45bfaf18fe1e7bb9a8939b.svg?invert_in_darkmode" align=middle width=40.79241539999999pt height=24.65753399999998pt/>, or the value of <img src="images/190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode" align=middle width=9.81741584999999pt height=22.831056599999986pt/> evaluated at each vertex of my tetrahedron. 

All of this means that a reasonable way to approximate any function in our tetrahedron is to use

<p align="center"><img src="images/478350d0d864b8692fe97555bff414b3.svg?invert_in_darkmode" align=middle width=138.01157369999999pt height=47.35857885pt/></p>

where <img src="images/2e26eb0900ac08bcaf445217a8686195.svg?invert_in_darkmode" align=middle width=35.29674884999999pt height=24.65753399999998pt/> are now the tetrahedron barycentric coordinates and <img src="images/9b6dbadab1b122f6d297345e9d3b8dd7.svg?invert_in_darkmode" align=middle width=12.69888674999999pt height=22.831056599999986pt/> are the values of <img src="images/190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode" align=middle width=9.81741584999999pt height=22.831056599999986pt/> at the nodes of the tetrahedron. Because our basis functions are linear, and the weighted sum of linear functions is still linear, this means that we are representing our function using a linear function space. 

## The Extension to 3D Movement

To apply this idea to physics-based animation of wiggly bunnies we need to more clearly define some of the terms above. First, we need to be specific about what our function <img src="images/190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode" align=middle width=9.81741584999999pt height=22.831056599999986pt/> will be. As with the particles in the previous assignments, what we care about tracking is the position of each mesh vertex, in the world, over time. For the <img src="images/c36ef0ba4721f49285945f33a25e7a45.svg?invert_in_darkmode" align=middle width=20.92202969999999pt height=26.085962100000025pt/> vertex we can denote this as <img src="images/07fa8f51f4cd0e31f63743c0638a3388.svg?invert_in_darkmode" align=middle width=56.34005519999999pt height=26.76175259999998pt/>. We are going to think of this value as a mapping from some undeformed space <img src="images/ae79425c6396a3fc7acd3d31d3bc3bc5.svg?invert_in_darkmode" align=middle width=54.86741699999999pt height=26.76175259999998pt/> into the real-world. So the function we want to approximate is <img src="images/0a6733943ebc99bb70de1c5cda4d14c5.svg?invert_in_darkmode" align=middle width=45.58205849999999pt height=26.085962100000025pt/> which, using the above, is given by

<p align="center"><img src="images/d39736c0e076aef46a625c5c707477b0.svg?invert_in_darkmode" align=middle width=154.83307785pt height=47.35857885pt/></p>

The take home message is that, because we evaluate <img src="images/c83e439282bef5aadf55a91521506c1a.svg?invert_in_darkmode" align=middle width=14.44544309999999pt height=22.831056599999986pt/>'s in the undeformed space, we need our tetrahedron to be embedded in this space. 

## The Generalized Coordinates 

Now that we have our discrete structure setup, we can start "turning the crank" to produce our physics simulator. A single tetrahedron has four (4) vertices. Each vertex has a single <img src="images/2982880f70a8a26b2a8893ae4db190b0.svg?invert_in_darkmode" align=middle width=14.942908199999989pt height=26.085962100000025pt/> associated with it. As was done in assignment 2, we can store these *nodal positions* as a stacked vector and use them as generalized coordinates, so we have

<p align="center"><img src="images/3ca0fe29482c50082a90537c92bcffc0.svg?invert_in_darkmode" align=middle width=77.84682345pt height=78.9048876pt/></p>

Now let's consider the velocity of a point in our tetrahedron. Given some specific <img src="images/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode" align=middle width=14.29216634999999pt height=22.55708729999998pt/>, the velocity at that point is

<p align="center"><img src="images/fcd1de7e861cf2d0a4a5d2a72aa12161.svg?invert_in_darkmode" align=middle width=186.53849279999997pt height=47.35857885pt/></p>

However, only the nodal variables actually move in time so we end up with

<p align="center"><img src="images/bb6065b2aa874eda6d41b8b2e1951471.svg?invert_in_darkmode" align=middle width=177.86275485pt height=47.35857885pt/></p>

Now we can rewrite this whole thing as a matrix vector product

<p align="center"><img src="images/fba0226e6f733da0926fe0f5f7a4b81e.svg?invert_in_darkmode" align=middle width=416.01854414999997pt height=103.61714219999999pt/></p>

where <img src="images/21fd4e8eecd6bdf1a4d3d6bd1fb8d733.svg?invert_in_darkmode" align=middle width=8.515988249999989pt height=22.465723500000017pt/> is the <img src="images/46e42d6ebfb1f8b50fe3a47153d01cd2.svg?invert_in_darkmode" align=middle width=36.52961069999999pt height=21.18721440000001pt/> Identity matrix. 

## The Kinetic Energy of a Single Tetrahedron

Now that we have generalized coordinates and velocities we can start evaluating the energies required to perform physics simulation. The first and, and simplest energy to compute is the kinetic energy. The main difference between the kinetic energy of a mass-spring system and the kinetic energy of an FEM system, is that the FEM system must consider the kinetic energy of every infinitesimal piece of mass inside the tetrahedron. 

Let's call an infinitesimal chunk of volume <img src="images/7ccb42e2821b2a382a72de820aaec42f.svg?invert_in_darkmode" align=middle width=21.79800149999999pt height=22.831056599999986pt/>. If we know the density <img src="images/6dec54c48a0438a5fcde6053bdb9d712.svg?invert_in_darkmode" align=middle width=8.49888434999999pt height=14.15524440000002pt/> of whatever our object is made out of, then the mass of that chunk is <img src="images/b393a71d7dbc7ba9bef53a45b498474d.svg?invert_in_darkmode" align=middle width=30.29688419999999pt height=22.831056599999986pt/> and the kinetic energy, <img src="images/2f118ee06d05f3c2d98361d9c30e38ce.svg?invert_in_darkmode" align=middle width=11.889314249999991pt height=22.465723500000017pt/> is <img src="images/e313402671b5400b40c904770c63159a.svg?invert_in_darkmode" align=middle width=140.34171855pt height=40.0576077pt/>. To compute the kinetic energy for the entire tetrahedron, we need to integrate over it's volume so we have

<p align="center"><img src="images/f20040204ebc8a72f628b655070e699b.svg?invert_in_darkmode" align=middle width=311.59084605pt height=37.3519608pt/></p>

BUT~ <img src="images/6877f647e069046a8b1d839a5a801c69.svg?invert_in_darkmode" align=middle width=9.97711604999999pt height=22.41366929999999pt/> is constant over the tetrahedron so we can pull that outside the integration leaving

<p align="center"><img src="images/63d88efc25d5db55efb8d46d2ca78bc2.svg?invert_in_darkmode" align=middle width=338.53143885pt height=63.53458154999999pt/></p>

in which the *per-element* mass matrix, <img src="images/f26134068b0b67870aaea679207afc4b.svg?invert_in_darkmode" align=middle width=22.18443314999999pt height=22.465723500000017pt/>, makes an appearance.. In the olden days, people did this integral by hand but now you can use symbolic math packages like *Mathematica*, *Maple* or even *Matlab* to compute its exact value. 

## The Deformation of a Single Tetrahedron

Now we need to define the potential energy of our tetrahedron. Like with the spring, we will need a way to measure the deformation of our tetrahedron. Since the definition of length isn't easy to apply for a volumetric object, we will try something else -- we will define a way to characterize the deformation of a small volume of space. Remember that all this work is done to approximate the function <img src="images/0a6733943ebc99bb70de1c5cda4d14c5.svg?invert_in_darkmode" align=middle width=45.58205849999999pt height=26.085962100000025pt/> which maps a point in the undeformed object space, <img src="images/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode" align=middle width=14.29216634999999pt height=22.55708729999998pt/>, to the world, or deformed space. Rather than consider what happens to a point under this mapping, let's consider what happens to a vector.  

To do that we pick two arbitary points in the undeformed that are infinitesimally close. We can call them <img src="images/5cb6ebd57b9c3ef9c5bf226aa856c60d.svg?invert_in_darkmode" align=middle width=20.84471234999999pt height=22.55708729999998pt/> and <img src="images/c89fe8937285e2a8ec46151e34cdff69.svg?invert_in_darkmode" align=middle width=20.84471234999999pt height=22.55708729999998pt/> (boring names I know). The vector between them is <img src="images/661cc435e9a81c8ee162d5ae09e11595.svg?invert_in_darkmode" align=middle width=107.98478954999997pt height=22.831056599999986pt/>. Similarly the vector between their deformed counterparts is <img src="images/266058ae232163eb4de3d7537b0fe02e.svg?invert_in_darkmode" align=middle width=154.29742514999998pt height=24.65753399999998pt/>. Because we chose the undeformed points to be infinitesimally close and <img src="images/f027e3c4319ee773dd95bd75d4e0408c.svg?invert_in_darkmode" align=middle width=159.81110475pt height=24.65753399999998pt/>, we can 
use Taylor expansion to arrive at

<p align="center"><img src="images/2a1f81d7a975db283cdc9e11ae4040d9.svg?invert_in_darkmode" align=middle width=119.45285385pt height=57.1636461pt/></p>

where <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/> is called the deformation gradient. Remember, <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/> results from differentiating a <img src="images/5dc642f297e291cfdde8982599601d7e.svg?invert_in_darkmode" align=middle width=8.219209349999991pt height=21.18721440000001pt/>-vector by another <img src="images/5dc642f297e291cfdde8982599601d7e.svg?invert_in_darkmode" align=middle width=8.219209349999991pt height=21.18721440000001pt/>-vector so it is a <img src="images/9f2b6b0a7f3d99fd3f396a1515926eb3.svg?invert_in_darkmode" align=middle width=36.52961069999999pt height=21.18721440000001pt/> matrix.

Because <img src="images/439d5b12dc6c860995693aa45e4255d1.svg?invert_in_darkmode" align=middle width=23.46465164999999pt height=22.831056599999986pt/> is pointing in an arbitrary direction, <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/>, captures information about how any <img src="images/439d5b12dc6c860995693aa45e4255d1.svg?invert_in_darkmode" align=middle width=23.46465164999999pt height=22.831056599999986pt/> changes locally, it encodes volumetric deformation. 

The FEM discretization provides us with a concrete formula for <img src="images/4ae122305a485974b9c14dcefe22cae8.svg?invert_in_darkmode" align=middle width=39.79437164999999pt height=24.65753399999998pt/> which can be differentiated to compute <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/>. *An important thing to keep in mind --* because our particular FEM uses linear basis functions inside of a tetrahedron, the deformation gradient is a constant. Physically this means that all <img src="images/439d5b12dc6c860995693aa45e4255d1.svg?invert_in_darkmode" align=middle width=23.46465164999999pt height=22.831056599999986pt/>'s are deformed in exactly the same way inside a tetrahedron.

Given <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/> we can consider the squared length of any <img src="images/74380e4b90b7786c87c490f3d94f2f68.svg?invert_in_darkmode" align=middle width=17.95095224999999pt height=22.831056599999986pt/> 

<p align="center"><img src="images/634073c68b4832cc0868f0293d1d432c.svg?invert_in_darkmode" align=middle width=406.03091759999995pt height=49.59096285pt/></p>

Like the spring strain, <img src="images/cbe99f908f3661c44ad20094523bbf90.svg?invert_in_darkmode" align=middle width=36.06344114999999pt height=27.6567522pt/> is invariant to rigid motion so it's a pretty good strain measure. 

## The Potential Energy of a Single Tetrahedron

The potential energy function of a tetrahedron is a function that associates a single number to each value of the deformation gradient. Sadly, for the FEM case, things are a little more complicated than just squaring <img src="images/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode" align=middle width=12.85392569999999pt height=22.465723500000017pt/> (but thankfully not much). 

### The Strain Energy density

Like the kinetic energy, we will begin by defining the potential energy on an infinitesimal chunk of the simulated object as <img src="images/1f4a642c645674854adffbaa639dd908.svg?invert_in_darkmode" align=middle width=94.0314837pt height=24.65753399999998pt/> where <img src="images/7e3c241c2dec821bd6c6fbd314fe4762.svg?invert_in_darkmode" align=middle width=11.29760774999999pt height=22.831056599999986pt/> is called the *strain energy density function. Mostly, we look up strain energy density functions in a book. Material scientists have been developing them for many years so that they mimic the behaviour of realistic materials. For this assignment you will use the well established, [Neo-Hookean](https://en.wikipedia.org/wiki/Neo-Hookean_solid) (its better than Hooke's Law because its new) strain energy density for compressible materials. This model approximates the behaviour of rubber-like materials. There are many formulations on that page, and we'll use the following:

<p align="center"><img src="images/74d82ca02624178772e80a255644d657.svg?invert_in_darkmode" align=middle width=308.68963289999994pt height=19.526994300000002pt/></p>

where <img src="images/44f83c3b5e0a3084115694e8bf6b75db.svg?invert_in_darkmode" align=middle width=81.08443199999998pt height=24.65753399999998pt/>.

The total potential of the tetrahedron can be defined via integration as 

<p align="center"><img src="images/1e9c29c7c5b33d59873273141163a333.svg?invert_in_darkmode" align=middle width=225.9949428pt height=37.3519608pt/></p>

### Numerical quadrature
Typically we don't evaluate potential energy integrals by hand. They get quite impossible, especially as the FEM basis becomes more complex. To avoid this we typically rely on [numerical quadrature](https://en.wikipedia.org/wiki/Numerical_integration). In numerical quadrature we replace an integral with a weighted sum over the domain. We pick some quadrature points <img src="images/9b01119ffd35fe6d8a8795a24fc11616.svg?invert_in_darkmode" align=middle width=18.943064249999992pt height=22.55708729999998pt/> (specified in barycentric coordinates for tetrahedral meshes) and weights <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/> and evaluate

<p align="center"><img src="images/7f5d5d26767ddfc8af4bc7e06443c91a.svg?invert_in_darkmode" align=middle width=164.90166989999997pt height=48.1348461pt/></p>

However, for linear FEM, the quadrature rule is exceedingly simple. Recall that linear basis functions imply constant deformation per tetrahedron. That means the strain energy density function is constant over the tetrahedron. Thus the perfect quadrature rule is to choose <img src="images/9b01119ffd35fe6d8a8795a24fc11616.svg?invert_in_darkmode" align=middle width=18.943064249999992pt height=22.55708729999998pt/> as any point inside the tetrahedron (I typically use the centroid) and <img src="images/c2a29561d89e139b3c7bffe51570c3ce.svg?invert_in_darkmode" align=middle width=16.41940739999999pt height=14.15524440000002pt/> as the volume of the tetrahedron. This is called *single point* quadrature because it estimates the value of an integral by evaluating the integrated function at a single point. 

## Forces and stiffness

The per-element generalized forces acting on a single tetrahedron are given by 

<p align="center"><img src="images/0a4b8813bf5f5396689dcc60d64ab92e.svg?invert_in_darkmode" align=middle width=72.3931461pt height=37.0084374pt/></p>

and the stiffness is given by 

<p align="center"><img src="images/846606bb011963601357e7ffbb1c2e4d.svg?invert_in_darkmode" align=middle width=87.95259164999999pt height=38.973783749999996pt/></p>

These can be directly computed from the quadrature formula above. Again, typically one uses symbolic computer packages to take these derivatives and you are allows (and encouraged) to do that for this assignment. 

For a tetrahedron the per-element forces are a <img src="images/1035a4ac6789d259e5b11678e1db3967.svg?invert_in_darkmode" align=middle width=44.748820049999985pt height=21.18721440000001pt/> vector while the per-element stiffness matrix is a dense, <img src="images/97ab0bce852d417ba9871adc2af7bb26.svg?invert_in_darkmode" align=middle width=52.968029399999985pt height=21.18721440000001pt/> matrix. 

## From a Single Tetrahedron to a Mesh 
Extending all of the above to objects more complicated than a single tetrahedron is analogous to our previous jump from a single spring to a mass-spring system. 

![A tetrahedral mesh](images/tet_mesh.png)

The initial step is to divide the object to be simulated into a collection of tetrahedra. Neighboring tetrahedra share vertices. We now specify the generalized coordinates of this entire mesh as 

<p align="center"><img src="images/cc44391faa5e8aa1e9178dd46a4b72ae.svg?invert_in_darkmode" align=middle width=79.37236725pt height=108.49566915pt/></p> 

where <img src="images/55a049b8f161ae7cfeb0197d75aff967.svg?invert_in_darkmode" align=middle width=9.86687624999999pt height=14.15524440000002pt/> is the number of vertices in the mesh. We use selection matrices (as we did in [assignment 2](https://github.com/dilevin/CSC417-a2-mass-spring-3d)) which yield identical assembly operations for the global forces, stiffness and mass matrix. 

## Time integration
Because you can compute all the necessasry algebraic operators (<img src="images/fb97d38bcc19230b0acd442e17db879c.svg?invert_in_darkmode" align=middle width=17.73973739999999pt height=22.465723500000017pt/>, <img src="images/d6328eaebbcd5c358f426dbea4bdbf70.svg?invert_in_darkmode" align=middle width=15.13700594999999pt height=22.465723500000017pt/>, <img src="images/47b0192f8f0819d64bce3612c46d15ea.svg?invert_in_darkmode" align=middle width=7.56844769999999pt height=22.831056599999986pt/>) you can use your linearly-implict Euler code from assignment 2 to integrate your FEM system. To see the limitations of this approach, run `a3-finite-elements-3d arma` and press `N`. Interacting with the armadillo will almost immediately cause your simulation to explode. This is because our bunny was secretly gigantic! So its effective stiffness was very low. This armadillo is much smaller (less than 1 meter tall). Even though it uses the same material parameters, its effective stiffness is much higher. Linearly-implicit Euler just cannot handle it, and so ... Kaboom! 

### Backward (Implicit) Euler

To fix this you will implement the full backward Euler integration scheme which will solve the discrete time stepping equations
<p align="center"><img src="images/078c28f2cd5f6e3559243bd7ebf55e0e.svg?invert_in_darkmode" align=middle width=264.9653985pt height=19.9563243pt/></p>

The position of the mesh is updated using <img src="images/3c7cac95de837f24405a387a05e1e94e.svg?invert_in_darkmode" align=middle width=87.07759334999999pt height=26.76175259999998pt/>. 

To solve for <img src="images/8c219eedfdfdc19ae90646c92cb8da87.svg?invert_in_darkmode" align=middle width=31.586827799999988pt height=26.76175259999998pt/> it is useful to notice that solving the update equation above is equivalent to the [optimization problem](https://www.google.com/search?client=safari&rls=en&q=Geometric+Numerical+Integration+Hairer&ie=UTF-8&oe=UTF-8)
<p align="center"><img src="images/b2cfa6fb033016e1698b121b3a85feec.svg?invert_in_darkmode" align=middle width=397.19944439999995pt height=34.59098115pt/></p>

We are going to solve this minimization problem using Newton's method. 

### Newton's method

[Newton's method](https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization) computes the local minimum of an objective function by iteratively solving a sequence of quadratic minimizations. Let's look at the process for a single iteration (say the <img src="images/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode" align=middle width=18.32504519999999pt height=27.91243950000002pt/> iteration). At this iteration we already have the <img src="images/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode" align=middle width=18.32504519999999pt height=27.91243950000002pt/> guess for our new velocity, denoted <img src="images/4ae0186e20725d789f0720e970fbca2b.svg?invert_in_darkmode" align=middle width=31.586827799999988pt height=28.894955100000008pt/>. The goal of the <img src="images/48a0115fc523b1aae58ade9e16001f59.svg?invert_in_darkmode" align=middle width=33.97362704999999pt height=21.68300969999999pt/> iteration is to compute a small change in the current velocity estimate,<img src="images/1606eff7b7a2241374521e03fe1b7239.svg?invert_in_darkmode" align=middle width=23.67578729999999pt height=22.465723500000017pt/>, that reduces the objective function.  

This is done by solving a local, quadratic optimization of the formula

<p align="center"><img src="images/96a0d4689a271ea009338df0fe7d3441.svg?invert_in_darkmode" align=middle width=372.8864172pt height=35.580073649999996pt/></p>

where <img src="images/5f3cc59831e6b4aef298a2dacada3fe7.svg?invert_in_darkmode" align=middle width=9.714576299999992pt height=14.611878600000017pt/> is the Gradient of the backward Euler integration cost function, and <img src="images/7b9a0316a2fcd7f01cfd556eedf72e96.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> is the Hessian.  In this case we have <img src="images/688dbb64333e268488085f69a78728c2.svg?invert_in_darkmode" align=middle width=277.86338414999994pt height=51.61677179999998pt/> and <img src="images/254b5869977f861ee43c7fd647de6abc.svg?invert_in_darkmode" align=middle width=210.9344292pt height=51.61677179999998pt/>

The solution to this problem is given by <img src="images/da730e32b8911b7e9ad3398f199274a0.svg?invert_in_darkmode" align=middle width=108.29894295pt height=26.76175259999998pt/>. You then compute <img src="images/e4a4c4648c0506be007da96bb84aa9de.svg?invert_in_darkmode" align=middle width=147.19636469999998pt height=28.894955100000008pt/>. 

Repeating the process of constructing and solving this quadratic optimization is at the heart of Newton's method.
<!--  -->


<!-- We start with the current state of our object (<img src="images/7cdb1bc71035910e68927b8821ecae9a.svg?invert_in_darkmode" align=middle width=14.942908199999989pt height=26.085962100000025pt/> and <img src="images/ef7188e17669f49e15672d7280b80119.svg?invert_in_darkmode" align=middle width=14.942908199999989pt height=26.085962100000025pt/>) and our goal is to compute <img src="images/5229a0b816a5e512d358e842aab8f4c4.svg?invert_in_darkmode" align=middle width=113.85812789999999pt height=26.76175259999998pt/>

Let's define the variable <img src="images/06084bd13aad92a3f657add21d5dc25e.svg?invert_in_darkmode" align=middle width=96.39232184999999pt height=26.085962100000025pt/>. We can Taylor expand our objective around this point, giving us

<p align="center"><img src="images/e5bb3e507ef64f233193c17770027989.svg?invert_in_darkmode" align=middle width=542.3851884pt height=34.59098115pt/></p> 

The solution to this minimization problem is found by solving

<p align="center"><img src="images/22b5ccb4ca14492a9521b55a317b12a3.svg?invert_in_darkmode" align=middle width=83.09339609999999pt height=14.42921205pt/></p>

where <img src="images/7b9a0316a2fcd7f01cfd556eedf72e96.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> is the Hessian of the above quadratic objective, and <img src="images/5f3cc59831e6b4aef298a2dacada3fe7.svg?invert_in_darkmode" align=middle width=9.714576299999992pt height=14.611878600000017pt/> is the gradient. 

Newton's method repeatedly computes <img src="images/1606eff7b7a2241374521e03fe1b7239.svg?invert_in_darkmode" align=middle width=23.67578729999999pt height=22.465723500000017pt/>, using it to update both <img src="images/e73485aa867794d51ccd8725055d03a3.svg?invert_in_darkmode" align=middle width=9.97711604999999pt height=14.611878600000017pt/> and <img src="images/6877f647e069046a8b1d839a5a801c69.svg?invert_in_darkmode" align=middle width=9.97711604999999pt height=22.41366929999999pt/>, until it either reaches some maximum number of iterations or finds the point for which the gradient of the full cost function is very close to zero.  -->

### Line Search

Unfortunately the <img src="images/1606eff7b7a2241374521e03fe1b7239.svg?invert_in_darkmode" align=middle width=23.67578729999999pt height=22.465723500000017pt/> computed by Newton's method can be overly ambitious causing the algorithm to never find a local minimum. To avoid this problem, robust optimization schemes give themselves the option of taking a fractional newton's step. One simple way to find a good step size is to use [backtracking line search](https://en.wikipedia.org/wiki/Backtracking_line_search). Line search is so named because it searches in the *direction* computed by Newton's method (along the line) but looks for a value of the full energy function that guarantees, for instance, sufficient decrease.  Backtracking line search gets its name because it initially tries to take a full Newton Step, and if that step is flawed, divides the step by some ratio and then checks the result of this new step. This procedure is repeated until either a suitable step is found, or the step length being checked goes to zero. 
        
## High Resolution Display Mesh via Skinning

The final component of this assignment involves making our FEM simulation look a bit more appealing. FEM calculations can be slow, especially if we want real-time performance. This often limits us to the use of relatively low resolution simulation meshes. To compensate for this we can adopt the concept of skinning from computer animation. 

Let us define two different meshes. The first is our simulation tetrahedral mesh with vertex positions given by <img src="images/fed8de6866c9a59fd93afcbfc65d0c96.svg?invert_in_darkmode" align=middle width=14.942908199999989pt height=26.085962100000025pt/>. The second is going to be a higher resolution triangle mesh, for display purposes. We will assume that our display mesh is completely enclosed by the simulation mesh when then simulation mesh is undeformed. 

Our goal is to transfer the motion of the simulation mesh to the display mesh. Remember that the FEM discretization defines the motion of our object, not just at the vertices, but everywhere inside the mesh (via the basis functions). Specifically, for any point in the undeformed space <img src="images/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode" align=middle width=14.29216634999999pt height=22.55708729999998pt/> I can reconstruct the deformed position <img src="images/0a6733943ebc99bb70de1c5cda4d14c5.svg?invert_in_darkmode" align=middle width=45.58205849999999pt height=26.085962100000025pt/> as long as I know which tetrahedron contains <img src="images/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode" align=middle width=14.29216634999999pt height=22.55708729999998pt/>. 

This gives us a simply algorithm to deform our display mesh. For each vertex in the display mesh (<img src="images/0d91a053c03a8131b1b1497899452125.svg?invert_in_darkmode" align=middle width=20.39667464999999pt height=22.55708729999998pt/>), find the tetrahedron, <img src="images/8cd34385ed61aca950a6b06d09fb50ac.svg?invert_in_darkmode" align=middle width=7.654137149999991pt height=14.15524440000002pt/>, that contains <img src="images/0d91a053c03a8131b1b1497899452125.svg?invert_in_darkmode" align=middle width=20.39667464999999pt height=22.55708729999998pt/> then move that vertex to position <img src="images/83cdc7684deaca981967b968b88bf53b.svg?invert_in_darkmode" align=middle width=36.48061889999999pt height=22.465723500000017pt/>. Here <img src="images/445351fa1f735b7fb15f43dc67563219.svg?invert_in_darkmode" align=middle width=19.444685699999987pt height=22.465723500000017pt/> and <img src="images/5d597e98cef80f07f6aac0d8e30cc70a.svg?invert_in_darkmode" align=middle width=16.214035199999987pt height=14.611878600000017pt/> are the basis function and generalized coordinates for the containing tetrahedron. 

This can be expresses as a linear operation 

<p align="center"><img src="images/4970b63427124bfb7d74f64ff5e5f8a7.svg?invert_in_darkmode" align=middle width=97.0097568pt height=24.31950015pt/></p> 

where <img src="images/96fc02c4358bf4203e3dad5a028f50ba.svg?invert_in_darkmode" align=middle width=60.52521419999998pt height=17.899602599999984pt/> are the deformed vertex positions of the display mesh and <img src="images/84c95f91a742c9ceb460a83f9b5090bf.svg?invert_in_darkmode" align=middle width=17.80826024999999pt height=22.465723500000017pt/> is the *skinning matrix* which contains the appropriate basis function values. 

## Assignment Implementation 

### Implementation Notes

For this course most functions will be implemented in **.cpp** files. In this assignment the only exception is that time integrators are implemented in **.h** files. This is due to the use of lambda functions to pass force data to the time integration algorithms. Finite element derivatives are both tricky and tedious to do correctly. Because of this you **DO NOT** have to take these derivatives by hand (unless you want to show off your mad skills). You can use a symbolic math package such as *Maple*, *Mathematica* or *Matlab*. **Note:** You can not use automatic differentiation, only symbolic math packages.

Running `a3-finite-elements-3d` will show a coarse bunny mesh integrated with linearly-implicit Euler. Running `a3-finite-elements-3d arma` will show a coarse armadillo mesh integrated using backward Euler. For each mesh you can toggle between the simulation and skinned meshes by pressing `S`. You can toggle integrators by pressing `N`. **Note:** the linearly-implicit integrator **WILL** explode when used with the armadillo mesh. 

### Important

Some of the functions from assignment 2 are reused here. If you correct implementation errors in those functions, your grades for the previous assignment will be updated to reflect that. 

### phi_linear_tetrahedron.cpp  

Evaluate the linear shape functions for a tetrahedron. This function returns a 4D vector which contains the values of the shape functions for each vertex at the world space point x (assumed to be inside the element).  

### dphi_linear_tetrahedron_dX.cpp  

Piecewise constant gradient matrix for linear shape functions. Row <img src="images/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> of the returned matrix contains the gradient of the <img src="images/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode" align=middle width=18.32504519999999pt height=27.91243950000002pt/> shape function.  

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



# Geometry Processing

## Overview of the repository

This repository contains three subprojects. The main subproject implements
several geometry processing algorithms grouped in a library called __geoproc__
(see directory [geoproc/](https://github.com/lluisalemanypuig/geometry-processing/tree/master/geoproc)).
The other two subprojects are a command line application that uses these algorithms
([command-line/](https://github.com/lluisalemanypuig/geometry-processing/tree/master/command-line)
directory), and a GUI ([interface/](https://github.com/lluisalemanypuig/geometry-processing/tree/master/interface)
directory).

## Contents

In the library __geoproc__ a set of algorithms for geometric processing have
been implemented. These algorithms include:
- Computation of the curvature of a triangular mesh:
	- Gauss curvature (Kg).
	- Mean curvature (Kh).
- Local smoothing algorithms: using one of these three operators
	- Laplacian.
	- Bi-Laplacian.
	- Taubin lambda-mu operator.
- Global smoothing algorithm: using only the Laplacian operator.
- Band frequencies.
- Remesing.
- Reflection lines.
- Harmonic Maps parametrisation.

## geoproc

The algorithms' implementations are inside the __geoproc/__ directory.
That directory also includes a small data structure for triangular meshes
and a simple parser of files _.ply_. The triangular mesh includes an algorithm
to compute its DCEL.

### Documentation

Documentation of the library was generated using [Doxygen](http://www.doxygen.nl/).
It can be found in the [geoproc-docs/](https://github.com/lluisalemanypuig/geometry-processing/tree/master/geoproc-docs)
directory.

## Dependencies

This project has several dependencies.

### Libraries

This project relies on the [glm](https://glm.g-truc.net/0.9.9/index.html)
(v. 0.9.5 or older) and the [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
libraries (v. 3.3.5 or older) for the geometry-processing algorithms.
Also, [OpenMP](http://www.openmp.org/) is needed for the parallelised
version of some of the algorithms.

The Qt library (tested only with version 5.9.6) to compile and execute the
interface to visualise the results of the algorithms.

### Tools

All subprojects of this repository rely on the _qmake_ tool for the generation
of the Makefiles, so it is needed (needless to say that the _make_ tool is
also required). Finally, the user will need a compiler that supports C++11
for all subprojects.

### Dependencies per project

All the projects in this repository use the _qmake_ tool for building.
Here are listed the dependencies of each subproject:

- __geoproc/__: depends on the _glm_ and _Eigen_ libraries.
- _command-line/_: depends on the glm library and the __geoproc__ library.
- _interface/_: depends on the Qt, _glm_, and __geoproc__ libraries.

## Compilation

It is strongly suggested that each subproject be compiled in _release_ mode.
All subprojects contain a _.pro_ file which can be opened using the IDE tool
Qt Creator. The _.pro_ files need to contain:

- The path to include the __geoproc__ library, and to link against it.
- The paths to include both the _glm_, and _Eigen_ (this last
one only when needed). The _.pro_ already include the include paths to these
libraries assuming they are installed in the system.

In order to successfully compile each subproject, one can follow these intructions:

- Create the makefile for every subproject

		qmake -makefile geoproc/geoproc.pro -o geoproc-release/Makefile
		qmake -makefile command-line/command-line.pro -o command-line-release/Makefile
		qmake -makefile interface/interface.pro -o interface-release/Makefile

- Compile first the geoproc library:

		cd geoproc-release/ && make

- Then the other two:
	
		cd command-line-release/ && make
		cd interface-release/ && make

### Linkage

Notice that the _command-line_ and _interface_ subprojects assume that the __geoproc__
library is placed and has been compiled into a directory placed within the same parent
folder and at the same level as theirs:
		
		geometry-processing:
			|-- geoproc/
			|-- geoproc-release/
			|-- command-line/
			|-- command-line-release/
			|-- interface/
			|-- interface-release/
			-

If the location of the geoproc library is to be changed, and compiled into a different
folder, then the _.pro_ files for the _command-line_ and _interface_ subprojects have
to be modified accordingly.

## Context

This repository was developed during the first semester of the academic
year 2018-2019 as part of the Geometry Processing course project from the
Master in Innovation and Research in Informatics (MIRI).

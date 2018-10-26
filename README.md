# Geometry Processing

Set of algorithms for geometric processing. These algorithms include:
- Computing the curvature of the vertices of a mesh:
	- Gauss curvature Kg
	- Mean curvature Kh
- Noise filtering:
	- Laplacian
	- Bi-Laplacian
	- Taubin lambda-mu operator

The algorithms' implementations are inside the _algorithms/_ directory.
That directory also includes a small data structure for triangular meshes
and a parser of files _.ply_.

These algorithms' result usually need to be displayed. In order to avoid
dumping the result onto a file and then having to open it with a 3D editor,
a small, interactive interface is provided so as to show these results.
One will find it inside the _interface/_ directory.

## Dependencies

This project has several dependencies.

### Libraries

This project relies on the [glm](https://glm.g-truc.net/0.9.9/index.html)
library (v. 0.9.5 or greater) for the geometry-processing algorithms and
the Qt (v. 5.9.6) library to compile and execute the interface to visualise
the results of the algorithms.

### Tools

All subprojects of this repository rely on the _qmake_ tool for the generation
of the Makefiles, so it is needed (needless to say that the _make_ tool is
also required). Finally, the user will need a compiler that supports C++11.

### Dependencies per project

All the projects in this repository use the _qmake_ tool for building.
However, some projects have _less_ dependencies than others:

- __algorithms/__: depends on the glm library and C++11.
- __command-line/__: depends on the glm library, C++11, and on the
_algorithms_ library.
- __interface/__: depends on the glm, Qt libraries, C++11, and on the
_algorithms_ library.

## Context

This repository was developed during the first semester of the academic
year 2018-2019 as part of the Geometry Processing course project from the
Master in Innovation and Research in Informatics (MIRI).

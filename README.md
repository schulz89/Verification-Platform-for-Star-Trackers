# Verification Platform for Star Trackers

This is a verification platform for testing star tracker algorithms, based on UVM-SystemC.  
All the code is under development and should be treated as alpha version.

## Compiling and running and running

Compiling the project with the supplied makefile was only tested on Fedora 29, with the versions of the dependencies shown in the list of dependencies below.

Compiling the platform can be done with:

```
make
```
Downloading the star catalog can be done with:

```
./download_hipparcos
```

Then, running the tests can be done with:

```
./run_test
```

## List of dependencies

* OpenCV 3.4.1
* Atlas 3.10.3
* Blas 3.8.0
* Boost 1.66.0
* SystemC 2.3.2
* UVM-SystemC 1.0b1
* GHDL (>= 0.35)
* Python 3.7.2
* NumPy  1.15.1 (for Python 3)
* Matplotlib 2.2.3 (for Python 3)

## License

This software is licensed under Apache License, Version 2.0

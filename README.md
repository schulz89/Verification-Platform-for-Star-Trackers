# Verification Platform for Star Trackers

This is a verification platform for testing star tracker algorithms, based on UVM-SystemC.  
The code is currently under development.

## Compiling and running and running

Compiling the project with the supplied makefile was tested on Debian 10, with the versions of the dependencies shown in the list of dependencies below.

Compiling the platform can be done with:

```
make
```
Downloading the star catalog can be done with:

```
./download_hipparcos
```

Ensure the Atlas library is being loaded:

```
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/atlas/
```

Then, running the tests can be done with:

```
./run_test
```

## List of dependencies

* OpenCV 3.2.0
* Atlas 3.10.3
* Blas 3.8.0
* Boost 1.67.0.1
* SystemC 2.3.3
* UVM-SystemC 1.0b2
* GHDL-mcode 0.35
* Python 3.7.3
* NumPy  1.16.2 (for Python 3)
* Matplotlib 3.0.2 (for Python 3)

## License

This software is licensed under Apache License, Version 2.0

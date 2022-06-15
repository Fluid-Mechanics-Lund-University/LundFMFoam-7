[![OpenFOAM version](https://img.shields.io/badge/OpenFOAM-7-brightgreen)](https://github.com/OpenFOAM/OpenFOAM-7)
# LundFMFoam
> `LundFMFoam` is an OpenFOAM tool package, including chemistry coordinate mapping (CCM) approach, Eulerian stochastic field (ESF) based transport probability density function (PDF) method.

The code is developed by [Bai's group](http://www.fm.energy.lth.se/english/) in Division of Fluid Mechanics, Lund University.

Here are the instructions for using `LundFMFoam`. The instructions are devided into two parts:
- If you are a user, only want to use the existing code, see [Guidelines for users](#guidelines-for-users).
- If you are a developer, planning to write your own solver or library based on `LundFMFoam`, see [Instructions for developer](#instructions-for-developer).

## Guidelines for users
1. Source the build-in OpenFOAM at first, for example
```shell
source $HOME/OpenFOAM/OpenFOAM-7/etc/bashrc
```

2. Download the `LundFMFoam` repository and source `bashrc` file
```shell
cd $HOME/OpenFOAM
git clone git@github.com:Fluid-Mechanics-Lund-University/LundFMFoam.git
cd LundFMFoam/etc
source bashrc
```

3. Once sourcing the `bashrc` file, you can compile the library or/and solver following the instructions

- compiling the libraries
```shell
srcLU
wclean all
./Allwmake
```

- compiling the app
```shell
appLU
wclean all
./Allwmake
```

The compiled binary files, such as solvers and `.so` libraries, are written into `$FOAM_USER_APPBIN` and `$FOAM_USER_LIBBIN`.

## Instructions for developer

We suggest that you develop your code in your own folder (e.g. `$WM_PROJECT_USER_DIR/app` and `$WM_PROJECT_USER_DIR/src`). Do not revise the offical `LundFMFoam` code when you develop your own code. Therefore, once you finished compiling the `LundFMFoam` (see [Guidelines for users](#guidelines-for-users)), change the permissions of `LundFMFoam` to read-only by commanding
```shell
chmod -R 444 $LU_PROJECT_DIR
```

1. Prepare the configuration files for compiling: `Make/files` and `Make/options` 
- In `Make/files`, set the target directory for binary files as `$FOAM_USER_APPBIN` or `$FOAM_USER_LIBBIN`. 
```C++
EXE = $(FOAM_USER_APPBIN)/libXXXFoam
LIB = $(FOAM_USER_LIBBIN)/libXXXModel
```
- In `Make/options`, use `$LU_SRC` and `$LU_APP` to set the include directories to find `.H` files in `LundFMFoam`. For example, the include files for `ESF` method are in `$LU_APP/app/solvers/ESFsprayFoam/lnInclude`. The include files for `CCM` approach are in `$LU_SRC/thermophysicalModels/chemistryModel/lnInclude`. Since the `LundFMFoam` libraries are written into `$FOAM_USER_LIBBIN`, you can either use `-L$(FOAM_USER_LIBBIN)/libCCMchemistryModel.so` or `-lCCMchemistryModel` to link the `LundFMFoam` libraries. Here is a demo for `Make/options`
```C++
EXE_INC = \
    -I$(LIB_SRC)/finiteVolume/lnInclude \
    -I$(LU_SRC)/thermophysicalModels/chemistryModel/lnInclude
    -I$(LU_APP)/app/solvers/ESFsprayFoam/lnInclude \

EXE_LIBS = \
    -lCCMChemistryModel \
    -lfiniteVolume
```


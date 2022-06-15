# The bash file for loading the LundFMFoam packages

## First, source the OpenFOAM
```shell
source $FOAM_BASH
```
## Then, source the `bashrc` file
```shell
source LundFMFoam/etc/bashrc
```

LU' is a abbreviation of LundFMFoam, after source the above two shell bash files, you will get:
- the export path of `LU_PROJECT_DIR`,`LU_SRC`,`LU_APP`,`LU_SOLVERS`,`LU_UTILITIES`,`LU_TUTORIALS`.
- the alias of `foamLU`,`srcLU`,`appLU`,`solLU`,`utilLU` and `tutLU` to jump to the corresponding directory.





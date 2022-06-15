# combustionModel

[![OpenFOAM version](https://img.shields.io/badge/OpenFOAM-7-brightgreen)](https://github.com/OpenFOAM/OpenFOAM-7)

## ESF

add `libs` in system/controlDict:
```
sed -i '$a\libs ("libESFCombustionModel.so");' system/controlDict
```

change the combustion model to ESF
```
foamDictionary -entry combustionModel -set ESF constant/combustionProperties
```

and add some lines in `combustionProperties`:
```
ESFCoeffs
{
    nStochasticFields       64;
    Cphi                    2;
    writeWienerSources      true;
    writeStochasticFields   false;
    dichotomicVectorsSym    false;
    solveMeanField          true;
    inertSpecieCorrection   true;
}
```

You may change:
- solveMeanField, using the ESF-O or the ESF-C method, refering to Ref. [1]. Suggestion is ESF-C, i.e. `ture`.
- inertSpecieCorrection, To get `sum(Yi) = 1`, either using the inert specie in thermophysicalProperties or `Yi = Yi/Sum(Yi)`. Suggestion is `ture`.

If you use solveMeanField to solve the element mass conservation, please also change the chemistrySolver to odeNoTruncation in `chemistryProperties`:

```
foamDictionary -entry chemistryType.chemistrySolver -set odeNoTruncation constant/chemistryProperties
```

[1] Xu S, Zhong S, Zhang F, Bai X S*. On the element mass conservation in Eulerian stochastic field modelling of turbulent combustion. Combustion and Flame, 2021.
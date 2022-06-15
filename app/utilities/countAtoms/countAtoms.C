/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Description
    Write the three components of the cell centres as volScalarFields so
    they can be used in postprocessing in thresholding.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "psiReactionThermo.H"
#include "BasicChemistryModel.H"
#include "reactingMixture.H"
#include "chemistrySolver.H"
#include "OFstream.H"
#include "thermoPhysicsTypes.H"
#include "basicSpecieMixture.H"
#include "chemkinReader.H"

//#include "readCCMProperties.H"

//#include "fvIOoptionList.H"


using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Main program:

int main(int argc, char *argv[])
{
    timeSelector::addOptions();

#   include "setRootCase.H"
#   include "createTime.H"

    instantList timeDirs = timeSelector::select0(runTime, args);

#   include "createMesh.H"

// create thermo environment and create pointerlist Y to all species scalarFields

OFstream atomCountS("atomCount.dat");
atomCountS<< "#%% Time	"<<"C	"<< "H	"<< "O	"<<"N	"<<"J_CmCO2	" << "J_HmH2O	" << "totalMass	" << endl;

autoPtr<psiReactionThermo> pThermo(psiReactionThermo::New(mesh));
psiReactionThermo& thermo = pThermo();
thermo.validate(args.executable(), "h", "e");

autoPtr< BasicChemistryModel<psiReactionThermo>> pChemistry
(
  BasicChemistryModel<psiReactionThermo>::New(thermo)
);

BasicChemistryModel<psiReactionThermo>& chemistry = pChemistry();
//*********************delete*********************************//
//basicMultiComponentMixture& composition = thermo.composition();
//************************************************************//
basicSpecieMixture& composition = thermo.composition();
//****************************************************//
PtrList<volScalarField>& Y = composition.Y();

PtrList<gasHThermoPhysics> specieData(Y.size());
forAll(specieData, i)
{
	specieData.set
	(
		i,
		new gasHThermoPhysics
		(
			dynamic_cast<const reactingMixture<gasHThermoPhysics>&>
				(thermo).speciesData()[i]
		)
	);
}

label  nSpecies = composition.Y().size();
List<List<specieElement> > specieComp(Y.size());
speciesTable speciesTab = thermo.composition().species();
autoPtr<chemistryReader<gasHThermoPhysics>> tchemRead
(
    chemistryReader<gasHThermoPhysics>::New(thermo,speciesTab)
);

const HashTable<List<specieElement> >& specComp(tchemRead().specieComposition());
forAll(specieComp,i)
{
    specieComp[i] = specComp[Y[i].name()];

}

wordList elementNames(4);

{
    elementNames[0] = "H";
    elementNames[1] = "C";
    elementNames[2] = "O";
    elementNames[3] = "N";
}

List<scalarField> spInfo(nSpecies);
forAll(spInfo,i)  spInfo[i].setSize(8,0);

forAll(Y, i)
{
    Info << "specie = " << Y[i].name() << "\t";
    forAll(elementNames, en)
    {
            forAll(specieComp[i], kk)
            {
                if(specieComp[i][kk].name() == elementNames[en])
                {
                    Info << specieComp[i][kk].name() << ", nAtoms = " << specieComp[i][kk].nAtoms() << "\t";
                    spInfo[i][en] = specieComp[i][kk].nAtoms();
                }
            }

    }

    Info << specieData[i].W() << endl;
    spInfo[i][4] = specieData[i].W();
}



scalar J_C = 0, J_H = 0, J_O = 0, J_N = 0, J_CmCO2 = 0, J_HmH2O = 0, totalMass = 0;

scalar weightC = 12.0;
scalar weightH = 1.008;
scalar weightO = 16.0;
scalar weightN = 14.0;

    forAll(timeDirs, timeI)
    {

        runTime.setTime(timeDirs[timeI], timeI);

        Info<< "Time = " << runTime.timeName() << endl;
        
        mesh.readUpdate();
        
		volScalarField rhoF
		(
			IOobject
			(
				"rho",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE

				),
			mesh
		);
		     
        J_C = 0, J_H = 0, J_O = 0, J_N = 0, J_CmCO2 = 0, J_HmH2O = 0, totalMass = 0;
        
        forAll(Y,i)
        {
		volScalarField phiYF
		(
			IOobject
			(
				Y[i].name(),
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::NO_WRITE

				),
			mesh
		);

				forAll(mesh.V(),cellI)
				 {
					// if(Z[cellI] > 1e-04)
					// {
						 J_C += weightC*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][1]/spInfo[i][4];
						 
						 J_H += weightH*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][0]/spInfo[i][4];
						 
						 J_O += weightO*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][2]/spInfo[i][4];
						 
						 J_N += weightN*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][3]/spInfo[i][4];
						 
						 totalMass += rhoF[cellI]*mesh.V()[cellI];
						 
						if( !Y[i].name().compare("H2O"))
						{
							J_HmH2O += weightH*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][0]/spInfo[i][4];
						}

						if( !Y[i].name().compare("CO2"))
						{
							J_CmCO2 += weightC*phiYF[cellI]*rhoF[cellI]*mesh.V()[cellI]*spInfo[i][1]/spInfo[i][4];
						}
					// }
				}
				
				
			}
				atomCountS<< runTime.timeName()<<"	"<<J_C <<"	"<< J_H << "	" << J_O << "	" << J_N << "	" << J_CmCO2 << "	" << J_HmH2O<< "	" << totalMass<< endl;
		}
    
    Info <<endl<< "End" << endl;

    return 0;
}


// ************************************************************************* //

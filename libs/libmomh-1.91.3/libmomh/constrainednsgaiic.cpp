/*
Multiple Objective MetaHeuristics Library in C++ MOMHLib++
Copyright (C) 2001 Andrzej Jaszkiewicz, Radoslaw Ziembinski (radekz@free.alpha.net.pl)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation (www.gnu.org); 
either version 2.1 of the License, or (at your option) any later 
version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef __CONSTRAINED_NSGAIIC_CPP_
#define __CONSTRAINED_NSGAIIC_CPP_

#include "tmotools.h"
#include "constrainednsgaiic.h"

template < class TProblemSolution > void TConstrainedNSGAIIC < TProblemSolution >::Run()
{
    vector < IntVector > iF;
    vector < double >dParentFitness;
    vector < double >dDistance;
    vector < double >dParentDistance;
    vector < IntVector > iSortTable;

    int i = 0;
    int j = 0;
    int k = 0;
    int iPopulation = 0;
    int iNumberToMove = 0;
    int iFrontSize = 0;
    int iFront = 0;
    double d = 0.0;
    double dDifference;
    int iCurrentGeneration = 0;
    int iSolution = 0;
    int iSolution1 = 0;
    int iSolution2 = 0;
    TProblemSolution *pParentA = NULL;
    TProblemSolution *pParentB = NULL;
    TProblemSolution *pOffspring = NULL;
    bool bExit = false;

    if (this->pNondominatedSet == NULL)
    {
        return;
    }

    this->pNondominatedSet->DeleteAll();

    this->Start();

    dParentFitness.resize(this->m_iPopulationSize);
    dParentDistance.resize(this->m_iPopulationSize);
    dDistance.resize(this->m_iDoubledPopulationSize); /*OD this->dDistance -> dDistance */

    // reserve memory for population
    this->m_oPPopulation.resize(this->m_iPopulationSize);
    this->m_oQPopulation.resize(this->m_iPopulationSize);
    this->m_oRPopulation.resize(this->m_iDoubledPopulationSize);

    // Prepare initial P and Q populations

    // Generate random P population
    for (i = 0; i < this->m_iPopulationSize; i++)
    {
        // generate random solution
        pParentA = new TProblemSolution();

        // store it in population
        this->m_oPPopulation[i] = pParentA;

        // update nondominated set
        if (this->pNondominatedSet->Update(*pParentA))
        {
            this->NewNondominatedSolutionFound();
        }

        NewSolutionGenerated(*(TProblemSolution *) pParentA);
    }

    // Fast nondominated sorting algorithm
    TMOTools::ConstrainedFastNondominatedSort(this->m_oPPopulation, iF);

    // clear fitness
    for (i = 0; i < this->m_iPopulationSize; i++)
    {
        dParentFitness[i] = 0.0;
    }

    // assign fitness from front list
    for (i = 0; i < iF.size(); i++)
    {
        for (j = 0; j < iF[i].size(); j++)
        {
            iSolution = iF[i][j];
            dParentFitness[iSolution] = i + 1.0;
        }
    }

    // debug
    for (i = 0; i < this->m_iPopulationSize; i++)
    {
        assert(dParentFitness[i] > 0.0);
    }

    // create Q population
    for (i = 0; i < this->m_iPopulationSize; i++)
    {
        iSolution1 = TMOTools::GetSolutionUsingBinaryTournamentMin(this->m_oPPopulation, dParentFitness);
        iSolution2 = TMOTools::GetSolutionUsingBinaryTournamentMin(this->m_oPPopulation, dParentFitness);

        // recombination                
        pOffspring =
            new TProblemSolution(*(TProblemSolution *) this->m_oPPopulation[iSolution1],
            *(TProblemSolution *) this->m_oPPopulation[iSolution2]);

        // mutation
        if (TMOTools::DoMutate(this->m_dMutationProbability) == true)
        {
            pOffspring->Mutate();
        }
        // update nondominated set
        if (this->pNondominatedSet->Update(*pOffspring) == true)
        {
            this->NewNondominatedSolutionFound();
        }
        // store offspring on new solution pull
        this->m_oQPopulation[i] = pOffspring;
        pOffspring = NULL;

        this->NewSolutionGenerated(*(TProblemSolution *) this->m_oQPopulation[i]);
    }

    // main algorithm loop
    iCurrentGeneration = 0;
    this->m_iGeneration = iCurrentGeneration;

    do
    {
        // join population P and Q to R
        for (i = 0; i < this->m_iPopulationSize; i++)
        {
            this->m_oRPopulation[i] = this->m_oPPopulation[i];
            this->m_oRPopulation[i + this->m_iPopulationSize] = this->m_oQPopulation[i];

            this->m_oPPopulation[i] = NULL;
            this->m_oQPopulation[i] = NULL;

            // clean variables
            dDistance[i] = 0.0;
            dDistance[i + this->m_iPopulationSize] = 0.0;
        }

        // fast nondominated sort on R population
	/** OD: This is a fundamental chage. I guess they forgot to change this. */
        TMOTools::ConstrainedFastNondominatedSort(this->m_oRPopulation, iF);
	// TMOTools::FastNondominatedSort(this->m_oRPopulation, iF);

        // compute aproximation of ideal nadir on R population
        // needed for scalarization
        if (this->m_bScalarize == true)
        {
            this->m_oRPopulation.UpdateIdealNadir();
        }

        for (iFront = 0; iFront < iF.size(); iFront++)
        {
            // last front is empty ommit empty fronts
            if (iF[iFront].size() == 0)
            {
                continue;
            }
            // sort solutions by objective
            // to compute crowding distance
            iSortTable.resize(0);

            iFrontSize = iF[iFront].size();
            TMOTools::QuickSortByObjectivesMax(this->m_oRPopulation, iF[iFront], iSortTable);

            for (i = 0; i < NumberOfObjectives; i++)
            {
                // ommit objectives that not active
                if (Objectives[i].bActive == false)
                {
                    continue;
                }

                iSolution = iSortTable[i][0];
                dDistance[iSolution] = -1.0;    // infinity

                iSolution = iSortTable[i][iFrontSize - 1];
                dDistance[iSolution] = -1.0;    // infinity                

                for (j = 1; j < iFrontSize - 2; j++)
                {
                    iSolution = iSortTable[i][j];

                    if (dDistance[iSolution] >= 0.0)
                    {
                        iSolution1 = iSortTable[i][j - 1];
                        iSolution2 = iSortTable[i][j + 1];

                        d = this->m_oRPopulation[iSolution1]->ObjectiveValues[i] -
                            this->m_oRPopulation[iSolution2]->ObjectiveValues[i];

                        // optional scalarization 
                        if (this->m_bScalarize == true)
                        {
                            dDifference =
                                this->m_oRPopulation.ApproximateIdealPoint.ObjectiveValues[i] -
                                this->m_oRPopulation.ApproximateNadirPoint.ObjectiveValues[i];
                            if (dDifference != 0.0)
                            {
                                d = d / dDifference;
                            } else
                            {
                                d = 0;
                            }
                        }

                        dDistance[iSolution] += abs(d);
                    }
                }
            }

            // sort solutions in the front from greatest distance
            // to lower
            TMOTools::QuickSortByValueMax(this->m_oRPopulation, dDistance, iF[iFront]);
        }

        // create new parent P population from till of R population
        // this is controlled aproach 
        // solutions are selected not from first front but from all
        iPopulation = 0;
        iNumberToMove = 0;

        for (iFront = 0; (iFront < iF.size()) && (iPopulation < this->m_iPopulationSize); iFront++)
        {
            if (iF[iFront].size() <= 0)
            {
                continue;
            }
            // geometrical sequence
            d = 1.0 - this->m_dPopulationGeometricalFactor;
            d = d / (1.0 - pow(this->m_dPopulationGeometricalFactor, iF.size() - 2.0));
            d = floor(this->m_oRPopulation.size() * d * pow(this->m_dPopulationGeometricalFactor, iFront));

            iNumberToMove += (int) d;

            if (iNumberToMove >= iF[iFront].size())
            {
                // if number of slots is greater then move whole population from front
                for (j = 0; (j < iF[iFront].size()) && (iPopulation < this->m_iPopulationSize); j++, iPopulation++)
                {
                    iSolution = iF[iFront][j];

                    assert(this->m_oRPopulation[iSolution] != NULL);

                    this->m_oPPopulation[iPopulation] = this->m_oRPopulation[iSolution];
                    dParentFitness[iPopulation] = iFront + 1.0;
                    dParentDistance[iPopulation] = dDistance[iSolution];

                    this->m_oRPopulation[iSolution] = NULL;
                }

                // reduce number of empty slots
                iNumberToMove -= iF[iFront].size();
            } else
            {
                // use tournament selection to choose the best solution
                for (j = 0; (j < iNumberToMove) && (iPopulation < this->m_iPopulationSize); j++, iPopulation++)
                {
                    iSolution1 = TMOTools::GetRandomIndex(this->m_oRPopulation, iF[iFront]);
                    iSolution2 = TMOTools::GetRandomIndex(this->m_oRPopulation, iF[iFront]);

                    assert(iSolution1 != -1);
                    assert(iSolution2 != -1);

                    if (dDistance[iSolution1] > dDistance[iSolution2])
                    {
                        assert(this->m_oRPopulation[iSolution1] != NULL);

                        this->m_oPPopulation[iPopulation] = this->m_oRPopulation[iSolution1];
                        dParentFitness[iPopulation] = iFront + 1.0;
                        dParentDistance[iPopulation] = dDistance[iSolution1];

                        this->m_oRPopulation[iSolution1] = NULL;
                    } else
                    {
                        assert(this->m_oRPopulation[iSolution2] != NULL);

                        this->m_oPPopulation[iPopulation] = this->m_oRPopulation[iSolution2];
                        dParentFitness[iPopulation] = iFront + 1.0;
                        dParentDistance[iPopulation] = dDistance[iSolution2];

                        this->m_oRPopulation[iSolution2] = NULL;
                    }
                }

                iNumberToMove = 0;
            }
        }

        // numerical errors can cause that geometrical sequence
        // doesn't cover whole  parent population
        if (iPopulation < this->m_iPopulationSize)
        {
            for (iFront = 0, k = 0; (iFront < iF.size()) && (iPopulation < this->m_iPopulationSize); iFront++)
            {
                for (j = 0; (j < iF[iFront].size()) && (iPopulation < this->m_iPopulationSize); j++, iPopulation++)
                {
                    iSolution = iF[iFront][j];
                    if (this->m_oRPopulation[iSolution] != NULL)
                    {
                        // move the first meet individual
                        this->m_oPPopulation[iPopulation] = this->m_oRPopulation[iSolution];
                        dParentFitness[iPopulation] = iFront + 1.0;
                        dParentDistance[iPopulation] = dDistance[iSolution];

                        this->m_oRPopulation[iSolution] = NULL;
                    }
                }
            }
        }
        // release rest of R population
        for (i = 0; i < this->m_iDoubledPopulationSize; i++)
        {
            if (this->m_oRPopulation[i] != NULL)
            {
                delete(TProblemSolution *) this->m_oRPopulation[i];
                this->m_oRPopulation[i] = NULL;
            }
        }

        // create Q population based on nisched operator
        for (i = 0; i < this->m_iPopulationSize; i++)
        {
            iSolution1 =
                TMOTools::GetSolutionUsingBinaryTournamentNischedMin(this->m_oPPopulation, dParentFitness,
                dParentDistance);
            iSolution2 =
                TMOTools::GetSolutionUsingBinaryTournamentNischedMin(this->m_oPPopulation, dParentFitness,
                dParentDistance);

	    /*	    if(iCurrentGeneration == 69)
	      {
		cout << "iSolution1: " << iSolution1 << " iSolution2: " << iSolution2 << endl;
	      }
	    */
            pParentA = (TProblemSolution *) this->m_oPPopulation[iSolution1];
            pParentB = (TProblemSolution *) this->m_oPPopulation[iSolution2];

	    // OD: pParentA and pParentB is not supposed to be NULL, but at times they are!!! I recover as below:
	    if(pParentA == NULL && pParentB != NULL)
	      {
		cerr << "Warning: pParentA is NULL!" << endl;
		pOffspring = new TProblemSolution(*pParentB);
	      }
	    else if(pParentA != NULL && pParentB == NULL)
	      {
		cerr << "Warning: pParentB is NULL!" << endl;
		pOffspring = new TProblemSolution(*pParentA);
	      }
	    else if(pParentA == NULL && pParentB == NULL)
	      {
		cerr << "Warning: pParentA and pParentB are NULL!" << endl;
		pOffspring = new TProblemSolution();
	      }
	    else
	      {
            // recombination                
            pOffspring = new TProblemSolution(*pParentA, *pParentB);
	      }

            // mutation
            if (TMOTools::DoMutate(this->m_dMutationProbability) == true)
            {
                pOffspring->Mutate();
            }
            // update nondominated set
            if (this->pNondominatedSet->Update(*pOffspring) == true)
            {
                this->NewNondominatedSolutionFound();
            }
            // store offspring on new solution pull
            this->m_oQPopulation[i] = pOffspring;
            pOffspring = NULL;

            NewSolutionGenerated(*(TProblemSolution *) this->m_oQPopulation[i]);
        }

        iCurrentGeneration += 1;
        this->m_iGeneration = iCurrentGeneration;

	/** OD */
	this->NewGenerationFound();
    }
    while ((this->bStop == false) &&
        ((this->bOnlineOfflineEvaluating == true) || (this->m_iGeneration < this->m_iNumberOfGenerations)));

    /** OD */
    cout << "bStop:" << this->bStop 
	 << " bOnlineOfflineEvaluating:" << this->bOnlineOfflineEvaluating
	 << " m_iGeneration:" << this->m_iGeneration
	 << " m_iNumberOfGenerations:" << this->m_iNumberOfGenerations << endl;
    // end callback
    this->End();

    // Cleanup object
    // Remove current population
    for (i = 0; i < this->m_iPopulationSize; i++)
    {
        if (this->m_oPPopulation[i] != NULL)
        {
            delete(TProblemSolution *) this->m_oPPopulation[i];
            this->m_oPPopulation[i] = NULL;
        }

        if (this->m_oQPopulation[i] != NULL)
        {
            delete(TProblemSolution *) this->m_oQPopulation[i];
            this->m_oQPopulation[i] = NULL;
        }
    }
}

template < class TProblemSolution > void TDisplayConstrainedNSGAIIC < TProblemSolution >::End()
{
    cout << "Optimization finished\n";
    cout << this->pNondominatedSet->iSetSize << '\t';
    int iobj;
    for (iobj = 0; iobj < NumberOfObjectives; iobj++)
    {
        if (Objectives[iobj].bActive)
        {
            cout << this->pNondominatedSet->ApproximateIdealPoint.ObjectiveValues[iobj] << '\t';
        }

        if (Objectives[iobj].bActive)
        {
            cout << this->pNondominatedSet->ApproximateNadirPoint.ObjectiveValues[iobj] << '\t';
        }
    }
    cout << '\n';
}

template < class TProblemSolution > void TDisplayConstrainedNSGAIIC < TProblemSolution >::NewNondominatedSolutionFound()
{
    cout << this->pNondominatedSet->iSetSize << '\t';
    int iobj;
    for (iobj = 0; iobj < NumberOfObjectives; iobj++)
    {
        if (Objectives[iobj].bActive)
        {
            cout << this->pNondominatedSet->ApproximateIdealPoint.ObjectiveValues[iobj] << '\t';
        }

        if (Objectives[iobj].bActive)
        {
            cout << this->pNondominatedSet->ApproximateNadirPoint.ObjectiveValues[iobj] << '\t';
        }
    }
    cout << '\n';
}

template < class TProblemSolution > void TDisplayConstrainedNSGAIIC < TProblemSolution >::NewGenerationFound()
{
    cout << "Generation: " << this->m_iGeneration + 1 << "\n";
}

#endif

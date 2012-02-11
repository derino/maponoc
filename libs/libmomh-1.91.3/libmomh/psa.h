/*
Multiple Objective MetaHeuristics Library in C++ MOMHLib++
Copyright (C) 2001 Andrzej Jaszkiewicz

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

// PSA.h: interface for the TPSA class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PSA_H_
#define __PSA_H_

#include "global.h"
#include "weights.h"
#include "momhsolution.h"
#include "nondominatedset.h"
#include "momethod.h"
#include "sabase.h"

/**    Pareto Simulated annealing
*
*    For more informations about the method see:
*    www-idss.cs.put.poznan.pl/~jaszkiewicz/PSA
*/
template < class TProblemSolution > class TPSA:public TSABase < TProblemSolution >
{
  protected:
    /** Defines magnitude of changes of weights in each iteration */
    double WeightsChangeCoefficient;

    /* Number of generating solutions */
    int GeneratingPopulationSize;

    /** Generating population */
    vector < TGeneratingPopulationElement * >GeneratingPopulation;

    /** Finds initial solutions and assigns weight vectors to them */
    void FindInitialPopulation();

    /** Updates weight vector of a given solution */
    void UpdateWeightVector(int isol);

  public:
    /** Sets default values of parameters */
    TPSA();

    /** Sets parameters of the method
    *
    *    Note that the total number of moves performed by all generating solutions
    *    on a given temperature level is equal to MovesOnTemperatureLevel * GeneratingPopulationSize */
    void SetParameters(double StartingTemperature,
        double FinalTemperature,
        double TemperatureDecreaseCoefficient,
        double WeightsChangeCoefficient, int MovesOnTemperatureLevel, int GeneratingPopulationSize);

    /** Runs the algorithm */
    virtual void Run();

};

/** Specialization of TPSA class displaying information
*    about progress of the optimization */
template < class TProblemSolution > class TDisplayPSA:public TPSA < TProblemSolution >
{
  protected:
    /** Displays:
    *    Information about the ned of optimization
    *    Number of potentially nondominated (Pareto-optimal) solutions
    *    Ideal point
    *    Nadir point
    */
    virtual void End();

    /** Displays:
    *    Number of potentially nondominated (Pareto-optimal) solutions
    *    Ideal point
    *    Nadir point
    */
    virtual void NewNondominatedSolutionFound();

    /** Displays:
    *    New temperature
    *    Percentage of accepted moves
    */
    virtual void TemperatureDecreased();


};

#include "psa.cpp"

#endif

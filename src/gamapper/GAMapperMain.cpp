/** 
  * Copyright (c) 2012, Universita' della Svizzera Italiana,
  * All rights reserved.
  * 
  * Redistribution and use in source and binary forms, with or without modification, 
  * are permitted provided that the following conditions are met:
  * 
  * 1. Redistributions of source code must retain the above copyright notice, this list 
  * of conditions and the following disclaimer.
  * 
  * 2. Redistributions in binary form must reproduce the above copyright notice, this 
  * list of conditions and the following disclaimer in the documentation and/or other 
  * materials provided with the distribution.
  * 
  * 3. Neither the name of Universita' della Svizzera Italiana nor the names of its 
  * contributors may be used to endorse or promote products derived from this software 
  * without specific prior written permission.
  * 
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  * SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
  * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 
  * The licenses for the third party libraries used in this software can be found in 
  * the folders of the third party libraries.
  *
  * Author(s): Onur Derin <oderin@users.sourceforge.net>
  * 
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>

#include <momh/mosa.h>
#include <momh/psa.h>
#include <momh/smosa.h>
#include <momh/mogls.h>
#include <momh/pma.h>
#include <momh/immogls.h>
#include <momh/momsls.h>
#include <momh/spea.h>
#include <momh/nsga.h>
#include <momh/nsgaii.h>
#include <momh/nsgaiic.h>
#include <momh/constrainednsgaiic.h>

#include <sigc++/sigc++.h>

#include "MappingMOMHSolution.hpp"
#include "MappingProblemReader.h"
#include "ReliabilityCalculator.h"
#include "gcc-double-comparison-fix.h"

#include "KPNTaskGraph.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;


//* Global variables declared as external in MappingMOMHSolution.cpp */
MappingProblem* theMP;
//ReliabilityCalculator* theRC;
void* rcInstance;
double* lambdaInstance;


/*
 * The optString global tells getopt_long() which options we
 * support, and which options have arguments.
 * 
 * The longOpts global tells getopt_long() which long options
 * we support, and which long options have arguments.
 */

struct globalArgs_t {
  double timeLimit;				// -t option
  int population;                                 // -P option
  int generations;                                 // -G option
  double crossOverRate;                              // -X option
  double mutationRate;                              // -M option
  char* appFileName;                            // -a option
  char* proFileName;                            // -p option
  char* arcFileName;                            // -r option
  MappingProblemType remapType;                 // -w option
  char* mapFileName;                            // -m option
  FaultTreeType ftType;                         // -c option
  char* frFileName;                             // -f option
  double compTimeThreshold;                     // -x option
} globalArgs;

static const char *optString = "t:P:G:X:M:a:p:r:w:m:c:f:x:h?";

static const struct option longOpts[] = {
	{ "time-limit", required_argument, NULL, 't' },
	{ "population", required_argument, NULL, 'P' },
	{ "generations", required_argument, NULL, 'G' },
	{ "cross-over-rate", required_argument, NULL, 'X'},
	{ "mutation-rate", required_argument, NULL, 'M' },
	{ "app", required_argument, NULL, 'a'},
	{ "pro", required_argument, NULL, 'p'},
	{ "arc", required_argument, NULL, 'r'},
	{ "remap-type", required_argument, NULL, 'w'},
	{ "map", required_argument, NULL, 'm'},
	{ "fault-tree-type", required_argument, NULL, 'c'},
	{ "fault-rates", required_argument, NULL, 'f'},
	{ "comp-time-threshold", required_argument, NULL, 'x'},
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};


time_t startingTime;

void usage()
{
  // rt: mttf calculation based on redundant tasks
  // tr: mttf calculation based on task remapping
  // remappers consider all fault scenarios
  // -rt considers different NMR-izations.
  cout << "Usage: gamapper [--time-limit <float>] [--population <int>] [--generations <int>] [--cross-over-rate <float>] [--mutation-rate <float>] --app appfile --pro profile --arc arcfile [--remap-type {limited|unlimited} --map mapfile] [--fault-tree-type {tr|rt} --fault-rates frfile] [--comp-time-threshold <float>]" << endl;

  cout << endl 
       << "Genetic algorithm options:" << endl
       << "--time-limit <float>      " << "\t\thow long the solver should run (in seconds), defaults to unlimited" << endl
       << "--population <int>        " << "\t\tnumber of individuals in the population, defaults to 1000" << endl
       << "--generations <int>       " << "\t\tnumber of generations, defaults to 100" << endl
       << "--cross-over-rate <float> " << "\t\tcross-over/recombination rate, between 0 and 1, defaults to 0.8" << endl
       << "--mutation-rate <float>   " << "\t\tmutation-rate, between 0 and 1, defaults to 0.001" << endl
       << endl
       << "Required arguments for the mapping problem:" << endl
       << "--app appfile             " << "\t\tapplication specification (Mte, d)"  << endl
       << "--pro profile             " << "\t\tprofiling specification (Ttc)"       << endl
       << "--arc arcfile             " << "\t\tarchitecture specification (Mnc, C)" << endl 
       << endl
       << "Additional arguments for the remapping problem" << endl
       << "--remap-type <type>       " << endl
       << "    limited               " << "\t\tlimited remapping (default)" << endl
       << "    unlimited             " << "\t\tunlimited remapping" << endl
       << "--map mapfile             " << "\t\tinitial mapping" << endl
       << "--faulty-nodes <nodelist>" << "\t\tTODO:comma-seperated list of faulty nodes (e.g. n1,n5)" << endl
       << "                          " << "\t\tdefaults to calculating remappings" << endl 
       << "                          " <<"\t\tfor all single fault scenarios"<<endl
       << endl
       << "Additional arguments for the reliability-aware mapping problem" << endl
       << "--fault-tree-type <type>  " << endl
       << "    tr                    " << "\t\tMTTF calculation based on task remapping (default)" << endl
       << "    rt                    " << "\t\tMTTF calculation based on redundant tasks" << endl
       << "--fault-rates frfile      " << "\t\tfault rate specification per node (lambda)" << endl
       << endl
       << "Objective options" << endl
       << "--comp-time-threshold <float>" << "\t\tthreshold for computation time of the application," << endl
       << "                             " << "\t\tused when we have a mapping problem with a throughput constraint," << endl
       << "                             " << "\t\tdefaults to no computation time threshold." << endl;
}

void print_args( void )
{
	printf("GAMapper is executed with the following parameters:\n");
	printf( "timeLimit: %f\n", globalArgs.timeLimit );
	printf( "population: %d\n", globalArgs.population );
	printf( "generations: %d\n", globalArgs.generations );
	printf( "cross-over rate: %f\n", globalArgs.crossOverRate );
	printf( "mutation rate: %f\n", globalArgs.mutationRate );
	printf( "appFileName: %s\n", globalArgs.appFileName );
	printf( "proFileName: %s\n", globalArgs.proFileName );
	printf( "arcFileName: %s\n", globalArgs.arcFileName );
	if (globalArgs.remapType == LIMITED_REMAPPING_PROBLEM)
		printf( "remapType: limited\n");
	else if(globalArgs.remapType == UNLIMITED_REMAPPING_PROBLEM)
		printf( "remapType: unlimited\n");
	else
		printf( "remapType: unset\n"); // This will never be the case.
	printf( "mapFileName: %s\n", globalArgs.mapFileName );

	if (globalArgs.ftType == RECONFIGURATION_WITH_TASK_REMAPPING)
		printf( "ftType: tr\n");
	else if(globalArgs.ftType == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS)
		printf( "ftType: rt\n");
	else
		printf( "ftType: unset\n"); // This will never be the case.
	printf( "frFileName: %s\n", globalArgs.frFileName );
	printf("compTimeThreshold: %f\n", globalArgs.compTimeThreshold);
}

bool verify_args()
{
	bool proper = false;
	if(globalArgs.appFileName != NULL && globalArgs.proFileName != NULL && globalArgs.arcFileName != NULL)
		proper = true;
	else
	{
		cout << "Error: Specify all three of the app, pro and arc files. e.g., -a input.app -p input.pro -r input.arc" << endl;
		return false;
	}

	// if ( (frFileName != NULL && ftType != NULL) || (frFileName == NULL && ftType == NULL) )
	// proper = true;
	// else return false;

	return proper;
}



  TNSGAII < MappingMOMHSolution > *solutionNSGAII;
  TNSGAIIC < MappingMOMHSolution > *solutionNSGAIIC;
  TNSGA < MappingMOMHSolution > *solutionNSGA;
  TConstrainedNSGAIIC<MappingMOMHSolution>* solutionConstrainedNSGAIIC;
  
  void StopAlgorithm(MOMHType iMomhType)
  {
    switch (iMomhType)
      {
      case MOMH_NSGAII:
	solutionNSGAII->StopAlgorithm();
	break;
      case MOMH_NSGAIIC:
	solutionNSGAIIC->StopAlgorithm();
              break;
      case MOMH_NSGA:
	solutionNSGA->StopAlgorithm();
	break;
      case MOMH_CONSTRAINEDNSGAIIC:
	solutionConstrainedNSGAIIC->StopAlgorithm();
	break;
      default:
	cout << "Error: Trying to stop an unknown Algorithm: " << iMomhType << endl;
      }
    
  }  
  
  void* timer(void* arg)
  {
    double* t = (double*) arg;

    int tsleep = (*t);

    cout << "Timer sleeping for " << tsleep << endl;
    sleep(tsleep);

    cout << "Time limit exceeded!" << endl;
    solutionConstrainedNSGAIIC->StopAlgorithm();
    //    StopAlgorithm(MOMH_CONSTRAINEDNSGAIIC);

    pthread_exit(NULL);
    return 0;

    //time_t currTime = time(NULL);
    
    //if (currTime - startingTime > globalArgs.timeLimit)
    //  {
    //	cout << "Time limit exceed!" << endl;
    //	StopAlgorithm(MOMH_CONSTRAINEDNSGAIIC);
    //  }
  }

/*#ifdef __cplusplus
extern "C"
{
#endif

static*/ void generation_found(int g, void *setPareto, void* setP, void* setQ)
  {
    cout << "generation: " << g << endl;

    std::stringstream g_str;
    g_str << g;

    // print current nondominated solution set
    ofstream foutPareto( ("pareto_set_gamapper_" + theMP->name() + "_gen_" + g_str.str() + ".tbl").c_str() );
    TNondominatedSet* paretoSet = (TNondominatedSet *) setPareto;
    for(std::vector<TSolution*>::iterator it=paretoSet->begin(); it != paretoSet->end(); it++)
      {
    	MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	if (msIt->Feasible())
	  foutPareto << "FeasiblePareto\t" << *msIt << endl;
	//else
	//  foutPareto << "Pareto\t" << *msIt << endl;
      }
    foutPareto.close();

    // print current P population
    ofstream foutP( ("population_P_" + theMP->name() + "_gen_" + g_str.str() + ".tbl").c_str() );
    TSolutionsSet* P = (TSolutionsSet*) setP;
    for(std::vector<TSolution*>::iterator it=P->begin(); it != P->end(); it++)
      {
    	MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	if (msIt->Feasible())
	  foutP << "FeasibleIndividual\t" << *msIt << endl;
      }
    foutP.close();
    
    // print current Q population
    ofstream foutQ( ("population_Q_" + theMP->name() + "_gen_" + g_str.str() + ".tbl").c_str() );
    TSolutionsSet* Q = (TSolutionsSet*) setQ;
    for(std::vector<TSolution*>::iterator it=Q->begin(); it != Q->end(); it++)
      {
    	MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	if (msIt->Feasible())
	  foutQ << "FeasibleIndividual\t" << *msIt << endl;
      }
    foutQ.close();    
  }


/*static*/ void solution_found(void *p)
  {
    // time_t currTime = time(NULL);
    // if(currTime-startingTime > 4)
    //   {
    // 	StopAlgorithm(MOMH_CONSTRAINEDNSGAIIC);
    // 	cout << "stopping." << endl;
    //   }
    // cout << "sol. found. @" << currTime-startingTime << endl;
    ofstream fout2( ("pareto_set_gamapper-" + theMP->name() + ".tbl").c_str() );
    // print to summary table
    TNondominatedSet *sol = (TNondominatedSet *) p;
    for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
      {
    	MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	if (msIt->Feasible())
	  fout2 << "FeasiblePareto\t" << *msIt << endl;
	//else
	//  fout2 << "Pareto\t" << *msIt << endl;
      }
    fout2.close();
    ////        signal_plugin.emit(sol);
  }

  /*#ifdef __cplusplus
}
#endif*/
  
  
  TNondominatedSet *solveProblem(MOMHType iMomhType/*, AlgorithmPreferences preferences*/, bool emit)
  {    
    startingTime = time(NULL);
    //int MovesOnTemperatureLevel;
    switch (iMomhType)
      {
      case MOMH_NSGAII:
	solutionNSGAII = new TNSGAII < MappingMOMHSolution >;
	if (emit)
	  solutionNSGAII->signal_new_nondominated_solution_found.connect(sigc::ptr_fun(solution_found));
	solutionNSGAII->SetParameters( 100, 10, 0.01, false
				      /*preferences.pareto_population_size,
				       preferences.pareto_number_of_generations, 
				       preferences.pareto_mutation_probability,
				       preferences.pareto_scalarize*/);
	cout << "NSGAII started..." << endl;
	solutionNSGAII->Run();
	cout << "NSGAII finished..." << endl;
	break;
      case MOMH_NSGAIIC:
	solutionNSGAIIC = new TNSGAIIC < MappingMOMHSolution >;
	if (emit)
	  solutionNSGAIIC->signal_new_nondominated_solution_found.connect(sigc::ptr_fun(solution_found));
	//solutionNSGAIIC->SetParameters(// preferences.pareto_population_size,
	// preferences.pareto_number_of_generations, // preferences.pareto_mutation_probability,
	// preferences.pareto_population_geometrical_factor, // preferences.pareto_scalarize);
	solutionNSGAIIC->Run();
	break;
      case MOMH_CONSTRAINEDNSGAIIC:
	solutionConstrainedNSGAIIC = new TConstrainedNSGAIIC < MappingMOMHSolution >;
	if (emit)
	  {
	    solutionConstrainedNSGAIIC->signal_new_nondominated_solution_found.connect(sigc::ptr_fun(solution_found));
	    solutionConstrainedNSGAIIC->signal_new_generation_found.connect(sigc::ptr_fun(generation_found));
	  }
	solutionConstrainedNSGAIIC->SetParameters(globalArgs.population, globalArgs.generations, globalArgs.mutationRate, globalArgs.crossOverRate, true //false
						  //100, 10, 0.01, 0.5, false
						  //1000, 100, 0.01, 0.8, false
						  // preferences.pareto_population_size,
						  // preferences.pareto_number_of_generations, 
						  // preferences.pareto_mutation_probability,
						  // preferences.pareto_population_geometrical_factor, 
						  // preferences.pareto_scalarize
						  );
	solutionConstrainedNSGAIIC->Run();
	break;
      case MOMH_NSGA:
	solutionNSGA = new TNSGA < MappingMOMHSolution >;
	if (emit)
	  solutionNSGA->signal_new_nondominated_solution_found.connect(sigc::ptr_fun(solution_found));
	//solutionNSGA->SetParameters(// preferences.pareto_population_size,
	// preferences.pareto_number_of_generations, // preferences.pareto_mutation_probability,
	// preferences.pareto_neighbour_distance, // preferences.pareto_scalarize);
	solutionNSGA->Run();
	break;
      default:
	return NULL;
	break;
      }
    time_t endingTime = time(NULL);
    cout << "Computing time: " << endingTime - startingTime << " seconds" << '\n';
    
    switch (iMomhType)
      {
      case MOMH_NSGAII:
	return solutionNSGAII->pNondominatedSet;
	break;
      case MOMH_NSGAIIC:
	return solutionNSGAIIC->pNondominatedSet;
	break;
      case MOMH_CONSTRAINEDNSGAIIC:
	return solutionConstrainedNSGAIIC->pNondominatedSet;
	break;
      case MOMH_NSGA:
	return solutionNSGA->pNondominatedSet;
	break;
      default:
	return NULL;
	break;
      }
    }
  
  // first L constraints are capacity constraints.
void capacityConstraints()
{
  // fill in the global variables of MOMH lib.
  // specify capacity constraints
  for (unsigned int iCon = 0; iCon < theMP->L(); iCon++) // NumberOfConstraints
    {
      Constraints[iCon].ConstraintType = _LowerThan;
      Constraints[iCon].Threshold = theMP->C();
    }
}

  /** read the problem and initialize objectives and constraints according 
      to the given command-line arguments
   */  
  void readProblem()
  {
    // read the problem
    MappingProblemReader* mpr = new MappingProblemReader();

    // these will be overriden later accordingly
    NumberOfObjectives = 0;

    // Houston, we have a mapping problem
    if(globalArgs.mapFileName == NULL && globalArgs.frFileName == NULL)
      {
	theMP = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName);
	theMP->mpType = MAPPING_PROBLEM;

	// define the problem
	// fill in the global variables of MOMH lib.
	// specify objectives
	NumberOfObjectives = 2;
	Objectives.resize(NumberOfObjectives);
	// comp.time objective
	Objectives[0].ObjectiveType = _Min;
	Objectives[0].bActive = true;
	// comm.cost objective
	Objectives[1].ObjectiveType = _Min;
	Objectives[1].bActive = true;

	// constraints
	NumberOfConstraints = theMP->L();
	Constraints.resize(NumberOfConstraints);
	capacityConstraints();
      }
    // we have a remapping problem
    else if(globalArgs.mapFileName != NULL && globalArgs.frFileName == NULL)
      {
	theMP = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName, globalArgs.mapFileName);
	theMP->mpType = globalArgs.remapType;

	cout << "Error: Remapping is not supported at the moment!" << endl;
	exit(EXIT_FAILURE);
      }
    // reliability-aware mapping problem
    else if(globalArgs.mapFileName == NULL && globalArgs.frFileName != NULL)
      {
	// specify objectives
	NumberOfObjectives = 3;
	Objectives.resize(NumberOfObjectives);
	// comp.time objective
	Objectives[0].ObjectiveType = _Min;
	Objectives[0].bActive = true;
	// comm.cost objective
	Objectives[1].ObjectiveType = _Min;
	Objectives[1].bActive = true;
	// mttf objective
	Objectives[2].ObjectiveType = _Max;
	Objectives[2].bActive = true;
	
	if (globalArgs.compTimeThreshold < 0) // ==-1 // normal problem with 3 objectives
	  {
	    theMP = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName, globalArgs.frFileName, globalArgs.ftType);
	    // set the global lambdaInstance acc. to the problem
	    RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (theMP);
	    lambdaInstance = rmp->lambda();

	    NumberOfConstraints = theMP->L();
	    Constraints.resize(NumberOfConstraints);
	    capacityConstraints();
	  }
	else  // RA mapping problem with throughput constraint (RA_TC_MAPPING_PROBLEM)
	  {
	    theMP = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName, globalArgs.frFileName, globalArgs.ftType, globalArgs.compTimeThreshold);
	    // set the global lambdaInstance acc. to the problem
	    RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*>(theMP);
	    lambdaInstance = rmp->lambda();
	    
	    // set comp objective as inactive (I tried adding comp. time both as objective and constraint but GA started giving strange seg. fault due to NULL solutions after 97. generations)
	    Objectives[0].bActive = false;
	    // add comp. time threshold as a constraint
	    NumberOfConstraints = rmp->L() + 1;
	    Constraints.resize(NumberOfConstraints);
	    capacityConstraints();
	    Constraints[rmp->L()].ConstraintType = _LowerThan;
	    Constraints[rmp->L()].Threshold = rmp->compTimeThreshold;
	  }

      }
    // reliability-aware remapping problem
    else // globalArgs.mapFileName != NULL && globalArgs.frFileName != NULL
      {
	cout << "Error: Reliability-aware remapping is not supported!" << endl;
	exit(EXIT_FAILURE);
      }

  }

void startTimeOverTimer()
  {
    pthread_t thread_id;
    int rc = 0;
    rc =  pthread_create(&thread_id, NULL, timer, (void *) &(globalArgs.timeLimit));
    if(rc)
      {    
	printf("=== Failed to create the timer thread\n");
	exit(EXIT_FAILURE);
      }
  }




  /**
     incrementally put tasks within a single fork-voter pattern.
     TODO: creating the fault tree for this case is more complicated. NOT IMPLEMENTED YET!
   */
void redundancyExplorer(RAMappingProblem* ramp)
{
  // save original app
  const int T_orig = ramp->T();
  const int E_orig = ramp->E();
  // deep copy ramp->Mte()
  int** Mte_orig = new int*[T_orig];
  for(int i=0; i<T_orig; i++)
    {
      Mte_orig[i] = new int[E_orig];
    }
  for (int i=0; i<T_orig; i++)
    for (int j=0; j<E_orig; j++)
      Mte_orig[i][j] = ramp->Mte()[i][j];

  // deep copy ramp->d()
  double* d_orig = new double[E_orig];
  for(int i=0; i<E_orig; i++)
    d_orig[i] = ramp->d()[i];

  // save original pro
  int CT_orig = ramp->CT();
  double** Ttc_orig = new double*[T_orig];
  for(int i=0; i<T_orig; i++)
    {
      Ttc_orig[i] = new double[CT_orig];
    }
  for (int i=0; i<T_orig; i++)
    for (int j=0; j<CT_orig; j++)
      Ttc_orig[i][j] = ramp->Ttc()[i][j];




  for (int i = 2; i< T_orig; i++) // for different redundancy coverages
    {
      // reset to the original problem
      ramp->setAppAndPro(T_orig, E_orig, Mte_orig, d_orig, CT_orig, Ttc_orig); //setApp deletes old ones,deep copies from new

      // apply NMR to first i tasks
      vector<string> vt;
      KPNTaskGraph g("1", ramp->T(), ramp->E(), ramp->Mte(), ramp->d(), ramp->CT(), ramp->Ttc() ); // TODO:ramp->Mtrt()

      for(int j=2; j<=i; j++)
	{
	  std::stringstream t_str;
	  t_str << "t" << j;
	  vt.push_back(t_str.str());
	}
      g.applyNMR(vt, 3);

      ramp->setAppAndPro(g.T(), g.E(), g.Mte(), g.d(), g.CT(), g.Ttc(), g.Mtrt());

      std::stringstream i_str;
      i_str << i;
      ramp->write("redundant-mp-"+i_str.str() ); // let's see if NMR is correct

      g.printDot("redundant-kpn-"+i_str.str()+".dot");

      
      // solve the ramp (= theMP)
      // TODO: set a filename prefix
      TNondominatedSet* sol = solveProblem(MOMH_CONSTRAINEDNSGAIIC, true);

      // print solutions
      cout << "Redundancy coverage " << i << " over " << T_orig << endl;
      for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
	{
	  cout << "Solution:" << endl;
	  MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	  if (msIt->Feasible())
	    cout << "FEASIBLE: ";
	  cout << *msIt << endl;
	}	 
            
      exit(EXIT_FAILURE); // testing fault tree for one redundancy level
    } // /END FOR different redundancy coverages
  
}



  /**
     incrementally put tasks within a fork-voter pattern of its own.
   */
void redundancyExplorer2(RAMappingProblem* ramp)
{
  // save original app
  const int T_orig = ramp->T();
  const int E_orig = ramp->E();
  // deep copy ramp->Mte()
  int** Mte_orig = new int*[T_orig];
  for(int i=0; i<T_orig; i++)
    {
      Mte_orig[i] = new int[E_orig];
    }
  for (int i=0; i<T_orig; i++)
    for (int j=0; j<E_orig; j++)
      Mte_orig[i][j] = ramp->Mte()[i][j];

  // deep copy ramp->d()
  double* d_orig = new double[E_orig];
  for(int i=0; i<E_orig; i++)
    d_orig[i] = ramp->d()[i];

  // save original pro
  int CT_orig = ramp->CT();
  double** Ttc_orig = new double*[T_orig];
  for(int i=0; i<T_orig; i++)
    {
      Ttc_orig[i] = new double[CT_orig];
    }
  for (int i=0; i<T_orig; i++)
    for (int j=0; j<CT_orig; j++)
      Ttc_orig[i][j] = ramp->Ttc()[i][j];



  // gercekte i=1, hizli debug icin i=2 yapiyorum
  for (int i = 9; i< T_orig; i++) // for different redundancy coverages (i==1 => no redundancy. also source and sink tasks are not NMRed.)
    {
      // reset to the original problem
      ramp->setAppAndPro(T_orig, E_orig, Mte_orig, d_orig, CT_orig, Ttc_orig); //setApp deletes old ones,deep copies from new

      // apply NMR to first i tasks
      KPNTaskGraph g("1", ramp->T(), ramp->E(), ramp->Mte(), ramp->d(), ramp->CT(), ramp->Ttc() ); // TODO:ramp->Mtrt()

      for(int j=2; j<=i; j++)
	{
	  vector<string> vt;
	  std::stringstream t_str;
	  t_str << "t" << j;
	  vt.push_back(t_str.str());
	  g.applyNMR(vt, 3);
	}

      ramp->setAppAndPro(g.T(), g.E(), g.Mte(), g.d(), g.CT(), g.Ttc(), g.Mtrt());

      std::stringstream i_str;
      i_str << i-1; // redundant-mp-1 means only one task is NMRed.
      ramp->setNamePrefix( "redundant-mp-"+i_str.str() );

      ramp->write( ramp->name() ); // let's see if NMR is correct

      g.printDot( ramp->name()+".dot" );

      // solve the ramp (= theMP)
      TNondominatedSet* sol = solveProblem(MOMH_CONSTRAINEDNSGAIIC, true);

      // print solutions
      cout << "Redundancy coverage " << i-1 << " over " << T_orig << endl;
      for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
	{
	  cout << "Solution:" << endl;
	  MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	  if (msIt->Feasible())
	    cout << "FEASIBLE: ";
	  cout << *msIt << endl;
	}
      // write solutions verbosely with the found mappings
      ofstream foutv( (ramp->name() + ".log").c_str() );
      foutv << "Detailed log of mapping solutions" << endl << endl;
      for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
	{
	  MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	  msIt->print(foutv);
	}
      foutv.close();

      //exit(EXIT_FAILURE); // testing fault tree for one redundancy level
    } // /END FOR different redundancy coverages
  
} // /END redundancyExplorer2

/*static*/ int execute()
  {
    /* initialize random seed: */
    //srand ( time(NULL) );

    cout << "GAMapper starting..." << endl;

    // read the problem, initializes theMP properly.
    cout << "reading problem...";
    readProblem();
    cout << "done" << endl;

    // create the global instance of ReliabilityCalculator, 
    //ReliabilityCalculator rc;
    //theRC = &rc; // theRC is the global variable. it is used in individuals to calculate their MTTF.
    //rcInstance = &rc; // rcInstance global variable has to point to the current rc instance at all times. (a trick for a static callback function when doing math. integration with Octave.)
    
    
    // if time limit is set, set a timer to stop after the given time limit
    if (globalArgs.timeLimit != -1)
      {
	startTimeOverTimer();
      }
    
    /////////////////////////////////////////////////////
    if(theMP->mpType == RA_MAPPING_PROBLEM 
       || theMP->mpType == RA_TC_MAPPING_PROBLEM)
      {
	RAMappingProblem* rmp = dynamic_cast< RAMappingProblem* >(theMP);
	
	if( rmp->ftType == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS)
	  {
	    // run GA several times with different redundancy coverages
	    cout << "runnig GA several times with different redundancy coverages..." << endl;
	    redundancyExplorer2(rmp); //redundancyExplorer(rmp);
	  } // /END RT
	else if( rmp->ftType == RECONFIGURATION_WITH_TASK_REMAPPING )
	  {
	    // future work: tr will change definition of failure to going below throughput constraint, i.e. beyond a degradation level. mttf will need to be calculated differently in that case. Then, it won't be dependent only on Mnc but on the remapping algorithm.
	    // for the moment, with tr, all mappings have the same MTTF.
	    TNondominatedSet* sol = solveProblem(MOMH_CONSTRAINEDNSGAIIC, true);
	    for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
	      {
		cout << "Solution:" << endl;
		MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
		if (msIt->Feasible())
		  cout << "FEASIBLE: ";
		cout << *msIt << endl;
	      }	 
	  } // /END TR
	else
	  {
	    cout << "Error: This should never print! Invalid fault-tree type!" << endl;
	    exit(EXIT_FAILURE);
	  }
      } // /END RA_TC_MAPPING_PROBLEM
    else if(theMP->mpType == MAPPING_PROBLEM 
	    || theMP->mpType == LIMITED_REMAPPING_PROBLEM 
	    || theMP->mpType == UNLIMITED_REMAPPING_PROBLEM ) 
      //REMAPPING_PROBLEM will be handled differently when implemented
      {
	TNondominatedSet* sol = solveProblem(MOMH_CONSTRAINEDNSGAIIC /*MOMH_NSGAII*/ /*MOMH_SPEA*/, true);
	for(std::vector<TSolution*>::iterator it=sol->begin(); it != sol->end(); it++)
	  {
	    cout << "Solution:" << endl;
	    MappingMOMHSolution* msIt = (MappingMOMHSolution*) *it;
	    if (msIt->Feasible())
	      cout << "FEASIBLE: ";
	    cout << *msIt << endl;
	  }	
      } // /END RE/MAPPING_PROBLEM
    else
      {
	cout << "Error: This should never print! Invalid mapping problem type!" << endl;
	exit(EXIT_FAILURE);
      }

    //////////////////////////////////////////////

    cout << "GAMapper finished!" << endl;
    return 0;
  }


int main( int argc, char *argv[] )
{
  // when comparing doubles, there was a strange behavior. it is solved with this: gcc-double-comparison-fix.h/.cpp
  // http://stackoverflow.com/questions/2497825/gcc-problem-with-raw-double-type-comparisons
  fpu_set_extended_precision_is_on(false);

  int opt = 0;
  int longIndex = 0;
  
  /* Initialize globalArgs before we get to work. */
  globalArgs.timeLimit = -1;
  globalArgs.population = 1000;
  globalArgs.generations = 100;
  globalArgs.crossOverRate = 0.8;
  globalArgs.mutationRate = 0.001;
  globalArgs.appFileName = NULL;
  globalArgs.proFileName = NULL;
  globalArgs.arcFileName = NULL;
  globalArgs.remapType = LIMITED_REMAPPING_PROBLEM;
  globalArgs.mapFileName = NULL;
  globalArgs.ftType = NO_RECONFIGURATION_WITH_REDUNDANT_TASKS;
  globalArgs.frFileName = NULL;
  globalArgs.compTimeThreshold = -1;

  
  /* Process the arguments with getopt_long(), then 
   * populate globalArgs. 
   */
  opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  while( opt != -1 ) {
    switch( opt ) {
    case 't':
      globalArgs.timeLimit = atof(optarg);
      break;
    case 'P':
      globalArgs.population = atoi(optarg);
      break;
    case 'G':
      globalArgs.generations = atoi(optarg);
      break;
    case 'X':
      globalArgs.crossOverRate = atof(optarg);
      break;
    case 'M':
      globalArgs.mutationRate = atof(optarg);
      break;
    case 'a':
      globalArgs.appFileName = optarg;
      break;
    case 'p':
      globalArgs.proFileName = optarg;
      break;
    case 'r':
      globalArgs.arcFileName = optarg;
      break;
    case 'w':
      if( strcmp(optarg, "limited")==0 )
	globalArgs.remapType = LIMITED_REMAPPING_PROBLEM;
      else if( strcmp(optarg, "unlimited")==0 )
	globalArgs.remapType = UNLIMITED_REMAPPING_PROBLEM;
      else {
	cout << "Error: Specify the remapping problem type. e.g., --remap-type limited OR --remap-type unlimited. Default value: limited" << endl;
	usage();
	return EXIT_FAILURE;
      }
      break;
    case 'm':
      globalArgs.mapFileName = optarg;
      break;
    case 'c':
      if( strcmp(optarg, "rt")==0 )
	globalArgs.ftType = NO_RECONFIGURATION_WITH_REDUNDANT_TASKS;
      else if( strcmp(optarg, "tr")==0 )
	globalArgs.ftType = RECONFIGURATION_WITH_TASK_REMAPPING;
      else {
	cout << "Error: Specify the fault-tree type. e.g., --fault-tree-type rt OR --fault-tree-type tr. Default value: rt. rt stands for NO_RECONFIGURATION_WITH_REDUNDANT_TASKS and tr stands for RECONFIGURATION_WITH_TASK_REMAPPING" << endl;
	usage();
	return EXIT_FAILURE;
      }
      break;
    case 'f':
      globalArgs.frFileName = optarg;
      break;
    case 'x':
      globalArgs.compTimeThreshold = atof(optarg);
      break;
    case 'h':	/* fall-through is intentional */
    case '?':
      usage();
      return EXIT_SUCCESS;
      break;
      
      /* case 0:		/\* long option without a short arg *\/ */
      /*   if( strcmp( "randomize", longOpts[longIndex].name ) == 0 ) { */
      /*     globalArgs.randomized = 1; */
      /*   } */
      /*   break; */
      
    default:	    /* You won't actually get here. */
      break;
    }
    
    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  }
  
  if( !verify_args() )
    {
      usage();
      return EXIT_FAILURE;
    }

  print_args();
  
  execute();
  
  return EXIT_SUCCESS;
}


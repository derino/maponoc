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

#include <fstream>

#include "LPMapper.h"
#include "MappingProblem.h"
//#include "MPEG2DecMappingProblem.h"
#include "MappingSolutionSet.h"
//#include "JPEGEncDecMappingProblem.h"
//#include "MatrixGenerator.h"
#include "MappingProblemReader.h"
ILOSTLBEGIN


void usage( void )
{
  puts( "Usage for mapper: lpmapper [--delta delta_value] [--time-limit time_limit [--gap-limit gap_limit]] --app app_file --pro pro_file --arc arc_file [--remap-type {limited|unlimited} --map map_file]" );
	/* ... */
	exit( EXIT_FAILURE );
}

/*void usageold()
{
  cout << "Usage for mapper: lpmapper -mp app_file pro_file arc_file" << endl;
  cout << "Usage for limited remapper: lpmapper -lrmp app_file pro_file arc_file map_file" << endl;
  cout << "Usage for unlimited remapper: lpmapper -urmp app_file pro_file arc_file map_file" << endl;

}*/


/*
 * The optString global tells getopt_long() which options we
 * support, and which options have arguments.
 * 
 * The longOpts global tells getopt_long() which long options
 * we support, and which long options have arguments.
 */

struct globalArgs_t {
  double delta;                                 // -d option
  double timeLimit;				// -t option
  double gapLimit;                              // -g option
  char* appFileName;                            // -a option
  char* proFileName;                            // -p option
  char* arcFileName;                            // -r option
  MappingProblemType remapType;                 // -w option
  char* mapFileName;                            // -m option
} globalArgs;

static const char *optString = "d:t:g:a:p:r:w:m:h?";

static const struct option longOpts[] = {
	{ "delta", required_argument, NULL, 'd' },
	{ "time-limit", required_argument, NULL, 't' },
	{ "gap-limit", required_argument, NULL, 'g'},
	{ "app", required_argument, NULL, 'a'},
	{ "pro", required_argument, NULL, 'p'},
	{ "arc", required_argument, NULL, 'r'},
	{ "remap-type", required_argument, NULL, 'w'},
	{ "map", required_argument, NULL, 'm'},
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

void print_args( void )
{
	printf("LPMapper is executed with the following parameters:\n");
	printf( "delta: %f\n", globalArgs.delta );
	printf( "timeLimit: %f\n", globalArgs.timeLimit );
	printf( "gapLimit: %f\n", globalArgs.gapLimit );
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

	if (globalArgs.gapLimit != 100 && globalArgs.timeLimit == 0)
	{
		cout << "Error: Specify either only time limit or both time limit and gap limit. e.g., -t 1 OR -t 1 -g 10" << endl;
		return false;
	}

	return proper;

}


void lp_mapper(double delta, double timeLimit, double gapLimit, MappingProblem* mp)
{
  // TEST MAPPING PROBLEM
  LPMapper lp(timeLimit, gapLimit, mp);
  MappingSolutionSet* mss = lp.epsilonConstraintMethod(delta);
      
  if (mss != NULL)
    {
      cout << "All calculated solutions: " << endl;
      mss->printObjValues(cout, *(mss->getSolutionSet()) );
      
      cout << "Pareto solutions: " << endl;
      vector<MappingSolution*>* pSet = mss->getParetoSet();
      mss->printObjValues(cout, *pSet );
      delete pSet;

/*      // write details in the log file.
      ofstream fout("pareto_set.log");
      mss->printAllValues(fout, *(mss->getParetoSet()) );
      fout.close();
*/
    }

  delete mss;
  return;
  // /TEST of LPMAPPER
}

// TEST of LPREMAPPER
void lp_remapper(double delta, double timeLimit, double gapLimit, MappingProblem* mp)
{

/** Printing of results to files are done inside MappingSolutionSet.add() as soon as new results are available. 
    No need for the file i/o parts below anymore.
    Differently from before, currently it requires an additional step to 'cat' all *.tbl files into one pareto_set_allfaults.tbl

  // summary table in a file
  ofstream fout("pareto_set_allfaults.tbl");
  fout << "F\tType\tExe.time\tComm.Cost" << endl;
*/

  MappingSolutionSet* mss = NULL;
  
  for (int i=1; i<= mp->N(); i++)
    {
      cout << "Calculating Pareto remappings for fault scenario n" << i << " faulty: " << endl;
      vector<int> vFaultyNodes;
      vFaultyNodes.push_back(i);
      mp->vFaultyNodes = &vFaultyNodes;
      RemappingProblem* rmp = dynamic_cast<RemappingProblem*>(mp);
      LPMapper lp(timeLimit, gapLimit, rmp/*, LIMITED_REMAPPING_PROBLEM*/ /*, &vFaultyNodes*/);
      mss = lp.epsilonConstraintMethod(delta);

/*
      if (mss != NULL)
	{
	  //cout << "All calculated solutions: " << endl;
	  //mss->printObjValues(cout, *(mss->getSolutionSet()) );
	  
	  vector<MappingSolution*>* pSet = mss->getParetoSet();
	  //cout << "Pareto solutions: " << endl;
	  //mss->printObjValues(cout, *pSet );

	  // print to summary table
	  for (vector<MappingSolution*>::iterator it = pSet->begin(); it != pSet->end(); it++)
	    {
	      if( (*it)->getStatus() == OPTIMAL_SOLUTION \/*IloAlgorithm::Optimal*\/  || (*it)->getStatus() == FEASIBLE_SOLUTION \/*LPMapper with limited time and GAMapper has non-optimal Pareto points, that's why FEASIBLEs are included.*\/)
		{
		  MappingSolution* ms = *it;
		  fout << i << "\tPareto\t" << ms->getCompCost() << "\t" << ms->getCommCost() << endl;
		}
	    }
	  
	  // write details in the log file.
	  std::stringstream i_str;
	  i_str << i;
	  ofstream fout2( ("pareto_set_faulty_n" + i_str.str() + ".log").c_str() );
	  mss->printAllValues(fout2, *(mss->getParetoSet()) );
	  fout2.close();
	}
*/
    }
  
//  fout.close();

  delete mss;
  return;
}

void execute () // old main
{
 // MappingProblem* mp_mpeg2dec3x3 = new MPEG2DecMappingProblem();
 // LPMapper lp(mp_mpeg2dec3x3);
 // MappingSolutionSet* mss = lp.epsilonConstraintMethod();
  
  //  MappingProblem* mp_mpeg2dec3x3 = new JPEGEncDecMappingProblem();
  //  MappingProblem* mp = new MatrixGenerator();

  MappingProblemReader* mpr = new MappingProblemReader();
  MappingProblem* mp = NULL;

  if(globalArgs.mapFileName == NULL) // Houston, we have a mapping problem
  {
	mp = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName);
	lp_mapper(globalArgs.delta, globalArgs.timeLimit, globalArgs.gapLimit, mp);
  }
  else  // we have a remapping problem
  {
	mp = mpr->read(globalArgs.appFileName, globalArgs.proFileName, globalArgs.arcFileName, globalArgs.mapFileName);
	mp->mpType = globalArgs.remapType;
	lp_remapper(globalArgs.delta, globalArgs.timeLimit, globalArgs.gapLimit, mp);
  }
/*  if(argc < 5)
    {
      usage();
      return -1;
    }  
  
  if(argc==5)
    {
      if( strcmp(argv[1], "-mp") == 0)
        mp = mpr->read(argv[2], argv[3], argv[4]);
      else
	{ usage(); return -1; }

      lp_mapper(mp);
    }
  else if(argc==6)
    {
      if( strcmp(argv[1], "-lrmp") == 0)
        mp = mpr->read(LIMITED_REMAPPING_PROBLEM, argv[2], argv[3], argv[4], argv[5]);
      else if( strcmp(argv[1], "-urmp") == 0)
        mp = mpr->read(UNLIMITED_REMAPPING_PROBLEM, argv[2], argv[3], argv[4], argv[5]);
      else
	{ usage(); return -1; }

      lp_remapper(mp);
    }
  else
    {
      usage();
      exit(-1);
    }
*/  
  //MappingSolution* ms = lp.map(8.50);
  //ms->print(cout);

  delete mpr;
  return;
}  // END execute


int main( int argc, char *argv[] )
{
	int opt = 0;
	int longIndex = 0;
	
	/* Initialize globalArgs before we get to work. */
	globalArgs.delta = 1.0;
	globalArgs.timeLimit = -1;
	globalArgs.gapLimit = 100;
	globalArgs.appFileName = NULL;
	globalArgs.proFileName = NULL;
	globalArgs.arcFileName = NULL;
	globalArgs.remapType = LIMITED_REMAPPING_PROBLEM;
	globalArgs.mapFileName = NULL;
	
	/* Process the arguments with getopt_long(), then 
	 * populate globalArgs. 
	 */
	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
	  switch( opt ) {
	  case 'd':
	    globalArgs.delta = atof(optarg);
	    break;
	  case 't':
	    globalArgs.timeLimit = atof(optarg);
	    break;
	  case 'g':
	    globalArgs.gapLimit = atof(optarg);
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
	    
	  case 'h':	/* fall-through is intentional */
	  case '?':
	    usage();
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


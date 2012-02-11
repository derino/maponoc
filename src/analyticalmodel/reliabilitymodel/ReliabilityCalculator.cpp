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
#define TODO

#include "ReliabilityCalculator.h"

//static string names[9] = {"n1", "n2", "n3", "n4", "n5", "n6", "n7", "n8", "n9"};
//static string names[6] = {"n1", "n2", "n4", "n5", "n6", "n8"};
//static string names[5] = {"n1", "n4", "n5", "n6", "n8"};

// failure rate of nodes
//static double lambda[9] = {0.00001, 0.00001, 0.00001, 0.00001, 0.0000001, 0.00001, 0.00001, 0.00001, 0.00001};

vector<bdd_assgn*> ReliabilityCalculator::vAssgn;

ReliabilityCalculator::ReliabilityCalculator()
{
  bdd_init(1000,100);
}

ReliabilityCalculator::~ReliabilityCalculator()
{
  bdd_done();
}

void ReliabilityCalculator::printhandler(ostream& o, int var)
{
  o << name(var);
//  o << names[var];
}

string ReliabilityCalculator::name(int var)
{
  std::stringstream var_str;
  var_str << "n" << var+1;
  return var_str.str();
}

void ReliabilityCalculator::allsatHandler(char* varset, int size)
{
  bdd_assgn* a = new bdd_assgn;
  a->varset = new char[size];
  memcpy(a->varset, varset, size);
  a->size = size;

  vAssgn.push_back(a);

#ifdef DEBUG
  cout << "assignments: ";
  for (int v = 0; v < size; ++v)
    {
      if (varset[v] == 1)
	{
	  cout << "+" << name(v) /*names[v]*/ << " ";
	}
      if (varset[v] == 0)
	{
	  cout << "-" << name(v) /*names[v]*/ << " ";
	}
    }
  cout << endl;
#endif
}

void ReliabilityCalculator::printFaultTree(RAMappingProblem* mp, MappingSolution* ms)
{
  // set the mapping problem
  this->mp = mp;
  
  if(mp->ftType == RECONFIGURATION_WITH_TASK_REMAPPING)
    createFaultTreeTR();
  else // type == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS
    createFaultTreeRT(ms);

  // calculate Sat. keep it in vAssgn vector.
  bdd_allsat(ft, ReliabilityCalculator::allsatHandler);


  cout << bddtable << ft << endl;
  cout << bddset << ft << endl;

if(mp->ftType == RECONFIGURATION_WITH_TASK_REMAPPING)
  {
    ofstream fout( ("fault_tree_bdd-" + mp->name() + ".dot").c_str() );
    fout << bdddot << ft << endl;
    fout.close();
  }
 else // type == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS
   {
     std::stringstream solId_str;
     solId_str << ms->uniqueID++;
     ofstream fout( ("fault_tree_bdd-" + mp->name() + "-pareto" + solId_str.str() + ".dot").c_str() );
     fout << bdddot << ft << endl;
     fout.close();
   }
 
  
  vAssgn.clear();
}

double ReliabilityCalculator::calculateMTTF(RAMappingProblem* mp /*, FaultTreeType type*/, MappingSolution* ms)
{ 
  // set the mapping problem
  this->mp = mp;

  if(mp->ftType == RECONFIGURATION_WITH_TASK_REMAPPING)
    createFaultTreeTR();
  else // type == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS
    createFaultTreeRT(ms);

  // calculate Sat. keep it in vAssgn vector.
  bdd_allsat(ft, ReliabilityCalculator::allsatHandler);

//  printFaultTree();


/** integration with GNU Scientific library */
	gsl_integration_workspace * w 
         = gsl_integration_workspace_alloc (1000);
       
       double result, error;
       //double expected = -4.0;
       //double alpha = 1.0;
     
       gsl_function F;
       F.function = &(ReliabilityCalculator::reliabilityFuncWrapper); //&f;
       F.params = NULL; //&alpha;
     
       gsl_integration_qags (&F, 0, 99999999, 0, 1e-7, 1000,
                             w, &result, &error); 
     
       //printf ("MTTF          = % .18f\n", result);
       ////printf ("exact result    = % .18f\n", expected);
       //printf ("estimated error = % .18f\n", error);
       ////printf ("actual error    = % .18f\n", result - expected);
       //printf ("intervals =  %d\n", w->size);
     
       gsl_integration_workspace_free (w);

/***** OCTAVE 

  // calculate MTTF using vAssgn
  DefQuad q(ReliabilityCalculator::reliabilityFuncWrapper, 0, 99999999 ); // INFINITY gives ABNORMAL RETURN FROM DQAGP MTTF: -nan //octave_Inf // octave_Float_Inf
  octave_idx_type ier;
  octave_idx_type neval;
  double abserr = 0;
  double result = q.do_integrate(ier, neval, abserr);
  //func(1);

/OCTAVE *****/

  //  cout << "MTTF: " << result << endl;

  // reset/clean bdd/memory
  //bdd_done();
  
  vAssgn.clear();
  return result;
}

/** 
    returns the node onto which the task with taskIndex is mapped
*/
int node(int taskIndex, int N, int** Xnt)
{
  for(int i=0; i<N; i++)
    {
      if(Xnt[i][taskIndex] == 1 )
	return i;
    }
  assert(false); // shouldn't execute
  return -1;
}

// DONE: redundant tasks - NMR with no reconfiguration
// detects the fault
// OR M-many of ANDs of all M-1 instances
// This is the version where failure is defined as the inability to 
// provide checked results.
void ReliabilityCalculator::createFaultTreeRT2(MappingSolution* ms)
{
  bdd* n = new bdd[mp->N()];

  //bdd_init(1000,100);
  bdd_strm_hook(ReliabilityCalculator::printhandler);

  bdd_setvarnum( mp->N() );

  for (int i=0; i<mp->N(); i++)
    n[i] = bdd_ithvar(i);

  ft = bdd_false();

  //int i=0;
  for ( map< int, vector<int> >::iterator tit = mp->Mtrt.begin(); tit!= mp->Mtrt.end(); ++tit )  
    //  for ( int i=0; i < mp->T(); i++ ) // for all tasks
    {
      bdd bdd_task_i = bdd_false();

      // if fork or voter, let's omit them for now
      if(tit->first == FORK_KEY || tit->first == VOTER_KEY )
	{
	  continue;
	}

      else if ( tit->second.size() == 1 ) // task has a single instance
	{
	  // if normal task with single instance, then simply OR
	  bdd_task_i = n[ node(tit->second[0], mp->N(), ms->getXnt() ) ];  // i -> tit->first
	}
      else // task has multiple instance (#M)
	{
	  // then OR M-many of ANDs of all M-1 instances
	  for (int r=0; r< tit->second.size(); r++ ) //for instance i in all instances
	    {
	      bdd bdd_task_i_allbutr = bdd_true();
	      for (int j = 0; j< tit->second.size(); j++)
		{
		  if (j != r)
		    bdd_task_i_allbutr &= n[ node( tit->second[j], mp->N(), ms->getXnt() ) ];
		}
	      bdd_task_i |= bdd_task_i_allbutr;
	    }
	}

      ft |= bdd_task_i;
      //i++;
    }
  
}

// This is the version where failure is defined as the inability to 
// provide any results.
// AND all redundant instances
void ReliabilityCalculator::createFaultTreeRT(MappingSolution* ms)
{
  bdd* n = new bdd[mp->N()];

  //bdd_init(1000,100);
  bdd_strm_hook(ReliabilityCalculator::printhandler);

  bdd_setvarnum( mp->N() );

  for (int i=0; i<mp->N(); i++)
    n[i] = bdd_ithvar(i);

  ft = bdd_false();

  //int i=0;
  for ( map< int, vector<int> >::iterator tit = mp->Mtrt.begin(); tit!= mp->Mtrt.end(); ++tit )  
    //  for ( int i=0; i < mp->T(); i++ ) // for all tasks
    {
      bdd bdd_task_i = bdd_true();

      // if fork or voter, let's omit them for now
      if(tit->first == FORK_KEY || tit->first == VOTER_KEY )
	continue;
      else if ( tit->second.size() == 1 ) // task has a single instance
	{
	  // if normal task with single instance, then simply OR
	  bdd_task_i = n[ ms->node(tit->second[0]) ];  // i -> tit->first
	}
      else // task has multiple instance (#M)
	{
	  // then AND all M-many instances
	  for (int r=0; r< tit->second.size(); r++ ) //for instance r in all instances
	    bdd_task_i &= n[ ms->node( tit->second[r]) ];
	}

      ft |= bdd_task_i;
      //i++;
    }
  
}



// DONE: online task remapping
void ReliabilityCalculator::createFaultTreeTR()
{
  bdd* n = new bdd[mp->N()];
  //bdd n1, n2, n3, n4, n5, n6, n7, n8, n9;
  //bdd_init(1000,100);
  bdd_strm_hook(ReliabilityCalculator::printhandler);

  //bdd_setvarnum(9);
  bdd_setvarnum( mp->N() );

  // n1 = bdd_ithvar(0);
  // n2 = bdd_ithvar(1);
  // n3 = bdd_ithvar(2);
  // n4 = bdd_ithvar(3);
  // n5 = bdd_ithvar(4);
  // n6 = bdd_ithvar(5);
  // n7 = bdd_ithvar(6);
  // n8 = bdd_ithvar(7);
  // n9 = bdd_ithvar(8);
  for (int i=0; i<mp->N(); i++)
    n[i] = bdd_ithvar(i);


  // ft = n1 | (n1 & n4 & n6) | n5 | n5 | n8;
  ft = bdd_false();
  for ( int i=0; i < mp->T(); i++ ) // for all tasks
    {
      bdd bdd_task_i = bdd_true();
      for ( int j=0; j < mp->CT(); j++ ) // for all core types
	{
	  bdd bdd_core_j = bdd_true();
	  if ( mp->Ttc()[i][j] != 0 && mp->Ttc()[i][j] != 1.79769e+308 /*TODO: should be Double::MAX*/ ) // if core type j can execute task i
	    {
	      // 'and' nodes that are of core type j
	      for ( int k=0; k < mp->N(); k++ )
		{
		  if ( mp->Mnc()[k][j] == 1 ) // if node k is of core type j
		    bdd_core_j &= n[k];
		}
	    }
	  bdd_task_i &= bdd_core_j;
	}
      ft |= bdd_task_i;
    }

}

// static wrapper-function to be able to callback the member function reliabilityFunc()
double ReliabilityCalculator::reliabilityFuncWrapper(/*void* pt2Self,*/ double t, void* params)
{
  // explicitly cast to a pointer to TClassA
  ReliabilityCalculator* mySelf = (ReliabilityCalculator*) rcInstance; //pt2Self;
  
  // call member
  return mySelf->reliabilityFunc(t);
}

// R(t)
double ReliabilityCalculator::reliabilityFunc(double t)
{
  double Q = 0; // failure probability of all paths (i.e. the assignment set of the BDD)
  double P[9];
  for (int i = 0; i<9; i++)
    P[i] = 1 - exp(-lambdaInstance[i]*t);

  for(vector<bdd_assgn*>::iterator it = vAssgn.begin(); it != vAssgn.end(); ++it)
    {
      bdd_assgn* a = (*it);
      double q_assgn = 1; // failure probability of one path 
      //TODO: indexler BDD'nin indexleri, NoC'un node indexleri degil. lambdalar ayniyken sorun cikarmaz ama sonra duzeltmek gerekir.
      for (int v = 0; v < a->size; ++v)
	{
	  if (a->varset[v] == 1)
	    {
	      q_assgn *= P[v]; 
#ifdef DEBUG	      
	      cout << "(1 - exp(-lambda[" << v << "]*t)) * "; //cout << "+" << names[v] << " ";
#endif
	    }
	  if (a->varset[v] == 0)
	    {
	      q_assgn *= (1.0-P[v]); 
#ifdef DEBUG	      
	      cout << "(exp(-lambda[" << v << "]*t)) * "; //cout << "-" << names[v] << " ";
#endif
	    }
	}
#ifdef DEBUG	      
      cout << " + " << endl; //cout << endl;
#endif
      Q += q_assgn;
    }

  // Q = (1-P[1]) * (1-P[2]) * (1-P[5]) * P[8];
  // Q += (1-P[1]) * (1-P[2]) * P[5];
  // Q += (1-P[1]) * P[2] * (1-P[4]) * (1-P[5]) * P[8];
  // Q += (1-P[1]) * P[2] * (1-P[4]) * P[5];
  // Q += (1-P[1]) * P[2] * P[4] * (1-P[5]) * (1-P[6]) * P[8];
  // Q += (1-P[1]) * P[2] * P[4] * (1-P[5]) * P[6];
  // Q += (1-P[1]) * P[2] * P[4] * P[5];
  // Q += P[1];
  return 1.0-Q;
}

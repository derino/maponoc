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
#include "MappingSolutionSet.h"

MappingSolutionSet::MappingSolutionSet()
{
}

MappingSolutionSet::~MappingSolutionSet()
{
  for(vector<MappingSolution*>::iterator it = s.begin(); it != s.end(); ++it)
    {
      delete (*it);
    }
}


vector<MappingSolution*>* MappingSolutionSet::getParetoSet()
{
  vector<MappingSolution*>* ps = new vector<MappingSolution*>();
  
  for (vector<MappingSolution*>::iterator it = s.begin(); it != s.end(); it++)
    {
      if( (*it)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || (*it)->getStatus() == FEASIBLE_SOLUTION)
	{
	  // check if *it is dominated. the check is done against those points that are
	  // already discovered as Pareto and those points that haven't been checked yet.
	  bool dominated = false;
	  for (vector<MappingSolution*>::iterator psit = ps->begin(); psit != ps->end(); psit++)
	    {
	      /*cout << "comparing " << *(*psit) << " with " << *(*it);*/
	      if ( (*psit)->dominatesEqual(*it) )
		{dominated = true; break; /*cout << " dominatedEqual";*/}
	      /*else cout << "not dominatedEqual";*/
	      /*cout << endl;*/
	    }
	  if ( it != s.end() && !dominated )
	    {
	      for (vector<MappingSolution*>::iterator sit = it+1; sit != s.end(); sit++)
		{
		  if( (*sit)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/)
		    {
		      /*cout << "comparing " << *(*sit) << " with " << *(*it);*/
		      if ( (*sit)->dominatesAbsolute(*it) )
			{dominated = true; break; /*cout << " dominatedAbsolute";*/}
		      /*else cout << "not dominatedAbsolute";*/
		      /*cout << endl;*/
		    }
		}
	      if(!dominated)
		{
		  ps->push_back(*it);
		  /*cout << "adding " << *(*it) << " as Pareto." << endl;*/
		}
	    }
	}
    }

  return ps;
}

vector<MappingSolution*>* MappingSolutionSet::getSolutionSet()
{
  return &s;
}

void MappingSolutionSet::add(MappingSolution* ms)
{
  s.push_back(ms);


  // to be able to see the results as the program runs, they are printed to file with every update to the solution set.
  // write details in the log file.
  // for a mapping problem, the standard name for the results file is "pareto_set.log", for a remapping problem, e.g., "pareto_set_N16-T30-faulty-n1.log"
  // this is not the most efficient way of doing this. 
  // (instead keep a pSet vector and update it with each add if ms is a Pareto. then append to the files.)

  vector<MappingSolution*>* pSet = this->getParetoSet();

  if (ms->getMP()->mpType == MAPPING_PROBLEM)
  {
	  ofstream fout("pareto_set.log");
	  this->printAllValues(fout, *pSet );
	  fout.close();

	  ofstream fout2("pareto_set.tbl");
	  // print to summary table
	  for (vector<MappingSolution*>::iterator it = pSet->begin(); it != pSet->end(); it++)
	    {
	      if( (*it)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || (*it)->getStatus() == FEASIBLE_SOLUTION /*LPMapper with limited time and GAMapper has non-optimal Pareto points, that's why FEASIBLEs are included.*/)
		{
		  MappingSolution* ms = *it;
		  fout2 << "Pareto\t" << *ms << endl; //ms->getCompCost() << "\t" << ms->getCommCost() << endl;
		}
	    }
	  fout2.close();
  }
  else if (ms->getMP()->mpType == LIMITED_REMAPPING_PROBLEM || ms->getMP()->mpType == UNLIMITED_REMAPPING_PROBLEM)
  {
	  ofstream fout( ("pareto_set_" + ms->getMP()->name() + ".log").c_str() );
	  this->printAllValues(fout, *pSet );
	  fout.close();

	  ofstream fout2( ("pareto_set_" + ms->getMP()->name() + ".tbl").c_str() );
	  // print to summary table
	  for (vector<MappingSolution*>::iterator it = pSet->begin(); it != pSet->end(); it++)
	    {
	      if( (*it)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || (*it)->getStatus() == FEASIBLE_SOLUTION /*LPMapper with limited time and GAMapper has non-optimal Pareto points, that's why FEASIBLEs are included.*/)
		{
		  MappingSolution* ms = *it;
		  fout2 << ms->getMP()->getFaultScenarioString() /*<< i*/ << "\tPareto\t" << *ms << endl; //ms->getCompCost() << "\t" << ms->getCommCost() << endl;
		}
	    }
	  fout2.close();
  }

  pSet->clear();
  delete pSet;
}

void MappingSolutionSet::printObjValues(ostream& os, vector<MappingSolution*>& v)
{
  for (vector<MappingSolution*>::iterator it = v.begin(); it != v.end(); it++)
    {
      if( (*it)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || (*it)->getStatus() == FEASIBLE_SOLUTION)
	os << *(*it) << endl;
    }
}

void MappingSolutionSet::printAllValues(ostream& os, vector<MappingSolution*>& v)
{
  os << "Detailed log of mapping solutions" << endl << endl;
  for (vector<MappingSolution*>::iterator it = v.begin(); it != v.end(); it++)
    {
      if( (*it)->getStatus() == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || (*it)->getStatus() == FEASIBLE_SOLUTION)
	{
	  (*it)->print(os);
	  os << endl << endl;
	}
      //os << *(*it) << endl;
    }
}


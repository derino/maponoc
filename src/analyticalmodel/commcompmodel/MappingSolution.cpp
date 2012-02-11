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
#include "MappingSolution.h"

int MappingSolution::uniqueID = 0;

MappingSolution::MappingSolution()
{
  
}

MappingSolution::~MappingSolution()
{
  if (Xnt != NULL)
    {
      for(int i=0; i<mp->N(); i++)
	delete[] Xnt[i];
      delete[] Xnt;
      Xnt = NULL;
    }
  else
    cout << "Warning: Double delete of Xnt avoided!" << endl;
 
  if (Ype != NULL)
    {
      for(int i=0; i<mp->P(); i++)
	delete[] Ype[i];
      delete[] Ype;
      Ype = NULL;
    }
  else
    cout << "Warning: Double delete of Ype avoided!" << endl;
}

MappingSolution::MappingSolution(MappingSolution& ms)
{
  
}

MappingSolution::MappingSolution(MappingProblem* _mp) : mp(_mp), status(UNKNOWN_SOLUTION/*IloAlgorithm::Unknown*/), solutionTime(0)
{
  Xnt = new int*[mp->N()];
  for (int i=0; i< mp->N(); i++)
    Xnt[i] = new int[mp->T()];

  for(int i=0; i<mp->N(); i++)
    for(int j=0; j<mp->T(); j++)
      Xnt[i][j] = 5;

  Ype = new int*[mp->P()];
  for (int i=0; i< mp->P(); i++)
    Ype[i] = new int[mp->E()];

  for(int i=0; i<mp->P(); i++)
    for(int j=0; j<mp->E(); j++)
      Ype[i][j] = 5;

  /*Tn = new double[mp->N()];*/
}

/*void MappingSolution::setCommCost(double c)
{
  commCost = c;
}*/

void MappingSolution::setStatus(MappingSolutionStatus s/*IloAlgorithm::Status s*/)
{
  status = s;
}

int** MappingSolution::getXnt()
{
  return Xnt;
}

int** MappingSolution::getYpe()
{
  return Ype;
}

/*double* MappingSolution::getTn()
{
  return Tn;
}*/

double MappingSolution::getCommCost()
{
  double commCost = 0;
  for(int i=0; i<mp->P(); i++)
    {
      int numLinksInPath = 0; // sum of 1 row in Mpl
      for(int k=0; k<mp->L(); k++)
	numLinksInPath = numLinksInPath + mp->Mpl()[i][k];
      
      for(int j=0;j<mp->E();j++)
	{
	  double dOnPath = (double)numLinksInPath * mp->d()[j];
	  if ( Ype[i][j] == 1 )
	    commCost += dOnPath;
	}
    }
  
  return commCost;
}

double MappingSolution::getCompCost()
{
  // calculate Tn as well

  // Ttn = Ttc Mcn
  double** Tnt = new double*[mp->N()];
  for(int i=0; i<mp->N(); i++)
    Tnt[i] = new double[mp->T()];
  
  for(int i=0; i<mp->N(); i++){
    for(int j=0; j<mp->T(); j++){
      Tnt[i][j] = 0.0;
    }
  }
  
  for(int i=0; i<mp->N(); i++){
    for(int j=0; j<mp->T(); j++){
      for(int k=0; k<mp->CT(); k++){
	Tnt[i][j] = Tnt[i][j] + mp->Ttc()[j][k] * mp->Mnc()[i][k] ;
      }
    }
  }

  // Tn = Tnt . Xnt element-wise and summed
  double* Tn = new double[mp->N()];
  for(int i=0; i < mp->N(); i++)
    Tn[i] = 0.0;

  for(int i=0; i < mp->N(); i++)
    for(int j=0; j<mp->T(); j++)
      Tn[i] = Tn[i] + Xnt[i][j] * Tnt[i][j];

  double max = 0;
  for (int i = 0; i < mp->N(); i++)
    {
      if (Tn[i] > max)
	max = Tn[i];
    }

  // free mem
  for(int i=0; i<mp->N(); i++)
    delete[] Tnt[i];
  delete[] Tnt;

  delete[] Tn;

  return max;
}

/*IloAlgorithm::Status*/MappingSolutionStatus MappingSolution::getStatus()
{
  return status;
}

/*IloNum*/double MappingSolution::getSolutionTime()
{
  return solutionTime;
}

void MappingSolution::setSolutionTime(/*IloNum*/double t)
{
  solutionTime = t;
}

double MappingSolution::getGap()
{
  return gap;
}

void MappingSolution::setGap(double g)
{
  gap = g;
}

MappingProblem* MappingSolution::getMP()
{
  return mp;
}

bool MappingSolution::dominatesEqual(MappingSolution* ms)
{
  return ( getCommCost() <= ms->getCommCost() 
	   && getCompCost() <= ms->getCompCost() );
}

bool MappingSolution::dominatesAbsolute(MappingSolution* ms)
{
  return ( (getCommCost() <= ms->getCommCost() 
	    && getCompCost() < ms->getCompCost())
	   ||
	   (getCommCost() < ms->getCommCost() 
	    && getCompCost() <= ms->getCompCost())
	   );
}

std::ostream& operator<<(std::ostream& os, MappingSolution& ms)
{
//  return os << "(" << ms.getCompCost() << ", " << ms.getCommCost() << ")";
//TODO: if RAMappingProblem, print also the MTTF value.
  return os << ms.getCompCost() << "\t" << ms.getCommCost();
}

void MappingSolution::print(ostream& out)
{
  out << "Solution status = " << status << endl;
  if (status == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || status == FEASIBLE_SOLUTION)
    {
      out << "Comm. cost  = " << getCommCost() << endl;
      out << "Comp. cost  = " << getCompCost() << endl;
      
      // out << "Tn        = " << endl;
      // out << "[ ";
      // for(int i = 0; i < mp->N(); i++)
      // 	{
      // 	  out << Tn[i] << " ";
      // 	}
      // out << "] " << endl;
      
      out << "Xnt        = " << endl;
      // printing Xnt in the format of a *.map file. It can be copy-pasted as a map file for further use (input to remapping or visualization)
      out << endl;
      out << mp->N() << endl;
      out << mp->T() << endl;
      out << endl;
      for(int i = 0; i < mp->N(); i++)
	{
	  //out << "| ";
	  for(int j = 0; j < mp->T(); j++)
	    {
	      out << Xnt[i][j] << " ";
	    }
	  out << endl; //out << "| " << endl;
	}
      out << endl;

	/* No need to print Ype
      out << "Ype        = " << endl;
      for(int i = 0; i < mp->P(); i++)
	{
	  out << "| ";
	  for(int j = 0; j < mp->E(); j++)
	    {
	      out << Ype[i][j];
	    }
	  out << "| " << endl;
	}*/
    } // end if optimal

  if (status == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || status == INFEASIBLE_SOLUTION/*IloAlgorithm::Infeasible*/ || status == FEASIBLE_SOLUTION)
    out << "Solution time = " << solutionTime << endl;
    out << "Gap = " << this->getGap() << endl;
}


std::ostream& operator<<(std::ostream& os, MappingSolutionStatus& msStatus)
{
    switch(msStatus)
    {
    case FEASIBLE_SOLUTION:
      return os << "FEASIBLE";
      break;
    case OPTIMAL_SOLUTION:
      return os << "OPTIMAL";
      break;
    case INFEASIBLE_SOLUTION:
      return os << "INFEASIBLE";
      break;
    case UNBOUNDED_SOLUTION:
      return os << "UNBOUNDED";
      break;
    case INFEASIBLE_OR_UNBOUNDED_SOLUTION:
      return os << "INFEASIBLE_OR_UNBOUNDED";
      break;
    case ERROR_SOLUTION:
      return os << "ERROR";
      break;
    default:
      return os << "UNKNOWN";
      break;
    }
}


/** 
    returns the node onto which the task with taskIndex is mapped
*/
int MappingSolution::node(int taskIndex)
{
  for(int i=0; i<mp->N(); i++)
    {
      if(Xnt[i][taskIndex] == 1 )
	return i;
    }
  assert(false); // shouldn't execute
  return -1;
}

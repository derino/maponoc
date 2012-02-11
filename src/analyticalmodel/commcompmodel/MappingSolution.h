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
#ifndef MAPPINGSOLUTION_H
#define MAPPINGSOLUTION_H

#include <iostream>
#include <cassert>
using namespace std;


//#include <ilcplex/ilocplex.h>
#include "MappingProblem.h"


enum MappingSolutionStatus 
{
  UNKNOWN_SOLUTION,
  FEASIBLE_SOLUTION,
  OPTIMAL_SOLUTION,
  INFEASIBLE_SOLUTION,
  UNBOUNDED_SOLUTION,
  INFEASIBLE_OR_UNBOUNDED_SOLUTION,
  ERROR_SOLUTION
};


class MappingSolution
{
 public:
  MappingSolution();
  ~MappingSolution();
  MappingSolution(MappingSolution& ms);
  MappingSolution(MappingProblem* mp);
  int** getXnt();
  int** getYpe();
  /*double* getTn();*/
  double getCommCost();
  //void setCommCost(double);
  double getCompCost();
  /*IloNum*/double getSolutionTime();
  void setSolutionTime(double/*IloNum*/);
  MappingSolutionStatus getStatus();
  //  IloAlgorithm::Status getStatus();
  void setStatus(MappingSolutionStatus);
  //  void setStatus(IloAlgorithm::Status);
  void setGap(double);
  double getGap();
  MappingProblem* getMP();
  
  bool dominatesAbsolute(MappingSolution*);
  bool dominatesEqual(MappingSolution*);
  void print(ostream&);

  static int uniqueID;

  // returns the node of task t according to Xnt
  int node(int t);

 protected:
  MappingProblem* mp;
  int** Xnt;
  int** Ype;
  //double* Tn;
  //double commCost;
  MappingSolutionStatus status;
  double gap; /* in percentages */
  //  IloAlgorithm::Status status;
  /*IloNum*/double solutionTime;

};

std::ostream& operator<<(std::ostream&, MappingSolution&);

std::ostream& operator<<(std::ostream&, MappingSolutionStatus& msStatus);

#endif

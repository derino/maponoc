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
#ifndef RAMAPPINGPROBLEM_H
#define RAMAPPINGPROBLEM_H

#include <map>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cassert>
using namespace std;

#include "MappingProblem.h"
#include "FaultRateSpecification.h"
//#include "ReliabilityCalculator.h"

typedef enum _FaultTreeType {RECONFIGURATION_WITH_TASK_REMAPPING, NO_RECONFIGURATION_WITH_REDUNDANT_TASKS} FaultTreeType;


class RAMappingProblem : public virtual MappingProblem, public virtual FaultRateSpecification
{
 public:
  FaultTreeType ftType;
  double compTimeThreshold;  // valid only if mpType==RA_TC_MAPPINGPROBLEM (reliability aware throughput constrained mapping problem)

  // Mtrt is valid only if ftType==NO_RECONFIGURATION_WITH_REDUNDANT_TASKS.
  // Mtrt[original task graph index i] = {list of indices in the redundant task graph of the redundant tasks of task i}
  map< int, vector<int> > Mtrt;

  virtual void setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc) = 0; // added due to applyNMR
  virtual void setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc, map< int, vector<int> > mtrt) = 0; // added due to applyNMR
  virtual void readTRT(char trtFile[256]);

  bool isForkTask(int t);
  bool isVoterTask(int t);
  vector<int> redundantsOfTask(int i);

  virtual void write(string filename);

  /*
  virtual int T() = 0; // #task
  virtual int E() = 0; // #edge in task graph
  virtual int N() = 0; // #node
  virtual int L() = 0; // #links in architecture graph
  virtual int P() = 0; // #path in architecture graph
  virtual int C() = 0; // max. link bandwidth
  virtual int** Mte() = 0;
  virtual int** Mpn() = 0;
  virtual int** Mpl() = 0;
  virtual double* d() = 0;

  virtual int CT() = 0; // #coretypes
  virtual double** Ttc() = 0;
  virtual int** Mnc() = 0;
  
  virtual int** Mnt() = 0;
  virtual double* lambda() = 0;*/
};

#endif

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
#ifndef RELIABILITYCALCULATOR_H
#define RELIABILITYCALCULATOR_H

//#define DEBUG

//#include <iomanip>
//#include <stdlib.h>
#include <bdd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>

#include <math.h>
#include <gsl/gsl_integration.h>
//#include <octave/oct.h>
//#include <octave/Quad.h>
//#include <octave/lo-ieee.h>

//#include "MappingProblem.h"
#include "RAMappingProblem.h"
#include "MappingSolution.h" // fault tree type RT depends on the mapping

using namespace std;

extern void* rcInstance; // this global pointer should always point to the correct instance.
extern double* lambdaInstance; // this points to the lambda values of the ramappingproblem.

//typedef enum _FaultTreeType {RECONFIGURATION_WITH_TASK_REMAPPING, NO_RECONFIGURATION_WITH_REDUNDANT_TASKS} FaultTreeType;

// satisfying assignment
typedef struct bdd_assignment {
  char* varset;
  int size;
} bdd_assgn;

class ReliabilityCalculator
{
 protected:
  RAMappingProblem* mp;
  bdd ft; // fault tree boolean expression
  static vector<bdd_assgn*> vAssgn; // Sat
  void createFaultTreeTR(); // RECONFIGURATION_WITH_TASK_REMAPPING
  void createFaultTreeRT(MappingSolution* ms); // NO_RECONFIGURATION_WITH_REDUNDANT_TASKS
  void createFaultTreeRT2(MappingSolution* ms); // NO_RECONFIGURATION_WITH_REDUNDANT_TASKS
  double reliabilityFunc(double); // R(t)
  static void printhandler(ostream& o, int var);
  static void allsatHandler(char* varset, int size);
  static string name(int var);
 public:
  ReliabilityCalculator();
  ~ReliabilityCalculator();
  double calculateMTTF(RAMappingProblem* mp/*, FaultTreeType*/, MappingSolution* ms);
  static double reliabilityFuncWrapper(double, void*);
  void printFaultTree(RAMappingProblem* mp, MappingSolution* ms);
};

#endif

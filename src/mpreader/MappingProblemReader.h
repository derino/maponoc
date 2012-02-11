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
  *            Deniz Kabakci <denizkabakci@yahoo.com>
  * 
  */
#ifndef MAPPINGPROBLEMREADER_H
#define MAPPINGPROBLEMREADER_H

#include <stdio.h>
#include <cstring>
//#include <conio.h>
#include <stdlib.h>
#include <math.h>
//#include <string>
#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <assert.h>

using namespace std;

#include "MappingProblem.h"
#include "ApplicationSpecification.h"
#include "ProfilingSpecification.h"
#include "ArchitectureSpecification.h"
#include "Mapping.h"
#include "FaultRateSpecification.h"
#include "RARemappingProblem.h"
#include "Link.h"
//#include "ReliabilityCalculator.h"

#define MAX_REC_LEN 1024

class MappingProblemReader :  public RARemappingProblem// , public virtual MappingProblem
{
public:
  MappingProblemReader();//char TGfile[256], char TCfile[256]);
  ~MappingProblemReader();

  ApplicationSpecification* readApp(char appFile[256]);
  ProfilingSpecification* readPro(char proFile[256]);
  ArchitectureSpecification* readArc(char arcFile[256]);
  Mapping* readMap(char mapFile[256]);
  FaultRateSpecification* readFR(char frFile[256]);
  MappingProblem* read(char appFile[256], char proFile[256], char arcFile[256]);
  RemappingProblem* read(char appFile[256], char proFile[256], char arcFile[256], char mapFile[256]);
  RAMappingProblem* read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType);
  RAMappingProblem* read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType, double _compTimeThreshold);
  RAMappingProblem* read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType, char trtFile[256]);

  // for now, only needed due to applyNMR
  virtual void setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc);
  virtual void setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc, map< int, vector<int> > mtrt);

  int T(); // #task
  int E(); // #edge in task graph
  int N(); // #node
  int L(); // #links in architecture graph
  int P(); // #path in architecture graph
  int C(); // max. link bandwidth
  int** Mte();
  int** Mpn();
  int** Mpl();
  double* d();
  int CT(); // #coretypes
  double** Ttc();
  int** Mnc();
  int** Mnt();
  double* lambda();

 protected:
  int **_Mte;
  double *_d;
  double** _Ttc; // Ttc directly read from second file
  int _E;		 
  int _T; 
  int _CT; 
  int _N;
  int _L;
  int _P;
  int _C; // OPTIONAL
  int **_Mpn;
  int **_Mpl;
  int **_Mnc;
  int** _Mnt;
  double* _lambda;

  Link *linksXdirection;
  Link *linksYdirection;

  int* coord(int node_num);

  void find(int coordinate1, int coordinate2, int tag, Link linksXdirection[], int pathnum);

  void path(int coord1, int coord2, int pathnum);
  
  void generateArchitectureMatrices();
  //  int generate(char TGfile[256], char TCfile[256]);
};

#endif

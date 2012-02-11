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
#include "MappingProblemReader.h"

MappingProblemReader::MappingProblemReader()
{
  //  generate(TGfile, TCfile);
  _Mte = NULL;
  _d = NULL;
  _Ttc = NULL;
  _Mpn = NULL;
  _Mpl = NULL;
  _Mnc = NULL;
  _Mnt = NULL;
  _lambda = NULL;

  linksXdirection = NULL;
  linksYdirection = NULL;
}

MappingProblemReader::~MappingProblemReader()
{
  // if an app_file was read
  if(_Mte != NULL)
  {
    for(int i=0; i<_T; i++)
      delete[] _Mte[i];
    delete[] _Mte;
  }

  // if a pro_file was read
  if(_d != NULL)
  {
    delete[] _d;
  }

  if(_Ttc != NULL)
  {
    for(int i=0; i<_T; i++)
      delete[] _Ttc[i];
    delete[] _Ttc;
  }

  // if an arc_file was read
  if(_Mpn != NULL)
  {
    for(int i=0; i<_P; i++)
      free(_Mpn[i]); //delete[] _Mpn[i];
    free(_Mpn); //delete[] _Mpn;
  }
  
  if(_Mpl != NULL)
  {
    for(int i=0; i<_P; i++)
      free(_Mpl[i]); //delete[] _Mpl[i];
    free(_Mpl); // delete[] _Mpl;
  }
  
  if(_Mpl != NULL)
  {
    for(int i=0; i<_N; i++)
      delete[] _Mnc[i];
    delete[] _Mnc;
  }
  
  if (_Mnt != NULL)
    {
      for(int i=0; i<_N; i++)
	delete[] _Mnt[i];
      delete[] _Mnt;
    }

  if(_lambda != NULL)
    delete[] _lambda;

  if(linksXdirection != NULL)
    free(linksXdirection);
  if(linksYdirection != NULL)    
    free(linksYdirection);
}

int MappingProblemReader::T()
{
  return _T;
}

int MappingProblemReader::E() // #edge in task graph
{
  return _E;
}

int MappingProblemReader::N() // #node
{
  return _N;
}

int MappingProblemReader::L() // #links in architecture graph
{
  return _L;
}

int MappingProblemReader::P() // #path in architecture graph
{
  return _P;
}

int MappingProblemReader::C() // max. link bandwidth
{
  return _C;
}

int** MappingProblemReader::Mte()
{
  return _Mte;
}

int** MappingProblemReader::Mpn()
{
  return _Mpn;
}

int** MappingProblemReader::Mpl()
{
  return _Mpl;
}

int MappingProblemReader::CT() // #coretypes
{
  return _CT;
}

double* MappingProblemReader::d()
{
  //  double* dd = new double;
  //*dd = (double) *_d;
  //return dd;  
  return _d;
}

double** MappingProblemReader::Ttc()
{
  return _Ttc;
}

int** MappingProblemReader::Mnc()
{
  return _Mnc;
}

int** MappingProblemReader::Mnt()
{
  return _Mnt;
}

double* MappingProblemReader::lambda()
{
  return _lambda;
}

ApplicationSpecification* MappingProblemReader::readApp(char appFile[256])
{
  fstream fin(appFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open file"; 
      exit(EXIT_FAILURE);
    }

  fin >> _T;
  fin >> _E;

  _Mte = new int*[_T];
  for(int i=0; i<_T; i++)
    {
      _Mte[i] = new int[_E];
    }

  for (int i=0; i<_T; i++)
    for (int j=0; j<_E; j++)
      _Mte[i][j] = 0;

  _d = new double[_E];

  for(int i=0; i<_E; i++)
    {
      int srcTask, dstTask;
      fin >> srcTask;
      fin >> dstTask;
      fin >> _d[i];
      
      _Mte[srcTask-1][i] = 1;
      _Mte[dstTask-1][i] = -1;
    }
  
    fin.close();
    
    return (ApplicationSpecification*)this;
}


void MappingProblemReader::setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc)
{
  //first delete memory for previous values, then deep copy new values
  // if an app_file was read
  if(_Mte != NULL)
  {
    for(int i=0; i<_T; i++)
      delete[] _Mte[i];
    delete[] _Mte;
  }

  // if a pro_file was read
  if(_d != NULL)
  {
    delete[] _d;
  }

  // if a pro_file was read
  if(_Ttc != NULL)
  {
    for(int i=0; i<_T; i++)
      delete[] _Ttc[i];
    delete[] _Ttc;
  }

  // set new values
  _T = t;
  _E = e;
  _CT = ct;
  
  _Mte = new int*[_T];
  for(int i=0; i<_T; i++)
    {
      _Mte[i] = new int[_E];
    }

  for (int i=0; i<_T; i++)
    for (int j=0; j<_E; j++)
      {
	//	cout << i << j << endl;
	_Mte[i][j] = mte[i][j];
      }
  
  _d = new double[_E];
  for(int i=0; i<_E; i++)
    _d[i] = D[i];


  _Ttc = new double*[_T];
  for(int i=0; i<_T; i++)
    {
      _Ttc[i] = new double[_CT];
    }
  
  for (int i=0; i<_T; i++)
    for (int j=0; j<_CT; j++)
      _Ttc[i][j] = ttc[i][j];

}

void MappingProblemReader::setAppAndPro(int t, int e, int** mte, double* D, int ct, double** ttc, map< int, vector<int> > mtrt)
{
  setAppAndPro(t, e, mte, D, ct, ttc);
  Mtrt = mtrt; // this is a deep copy
}

ProfilingSpecification* MappingProblemReader::readPro(char proFile[256])
{
  fstream fin(proFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open file"; 
      exit(EXIT_FAILURE);
    }

  fin >> _CT;

  _Ttc = new double*[_T];
  for(int i=0; i<_T; i++)
    {
      _Ttc[i] = new double[_CT];
    }

  for(int i=0; i<_T; i++)
    for(int j=0; j<_CT; j++)
      fin >> _Ttc[i][j];

  fin.close();

  return (ProfilingSpecification*)this;
}

ArchitectureSpecification* MappingProblemReader::readArc(char arcFile[256])
{
  fstream fin(arcFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open file"; 
      exit(EXIT_FAILURE);
    }

  fin >> _N;
  fin >> _CT;
  fin >> _C;

  _Mnc = new int*[_N];
  for(int i=0; i<_N; i++)
    {
      _Mnc[i] = new int[_CT];
    }

  for(int i=0; i<_N; i++)
    for(int j=0; j<_CT; j++)
      fin >> _Mnc[i][j];

  generateArchitectureMatrices();

  // if the architecture has forkvoter nodes, set the forkVoterNodes accordingly.
  // if arc file ends with "first_column_is_forkvoter", then first column is the forkvoter coretype.
  string s;
  fin >> s;
  if(!fin)
    fin.clear();
  else
    {
      assert(s == "first_column_is_forkvoter");
      for(int i=0; i<_N; i++)
	{
	  if(_Mnc[i][0] == 1)
	    {
	      forkVoterNodes.push_back(i);
	      cout << "ForkVoter Node:" << i << endl;
	    }
	}
    }
  
  return (ArchitectureSpecification*)this;
}


Mapping* MappingProblemReader::readMap(char mapFile[256])
{
  fstream fin(mapFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open file"; 
      exit(EXIT_FAILURE);
    }

  fin >> _N;
  fin >> _T;

  _Mnt = new int*[_N];
  for(int i=0; i<_N; i++)
    {
      _Mnt[i] = new int[_T];
    }

  for(int i=0; i<_N; i++)
    for(int j=0; j<_T; j++)
      fin >> _Mnt[i][j];

  return (Mapping*)this;
}

FaultRateSpecification* MappingProblemReader::readFR(char frFile[256])
{
  fstream fin(frFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open file"; 
      exit(EXIT_FAILURE);
    }

  fin >> _N;

  _lambda = new double[_N];

  for(int i=0; i<_N; i++)
      fin >> _lambda[i];
  
  return (FaultRateSpecification*)this;  
}



MappingProblem* MappingProblemReader::read(char appFile[256], char proFile[256], char arcFile[256])
{
  mpType = MAPPING_PROBLEM;
  vFaultyNodes = NULL;
  readApp(appFile);
  readPro(proFile);
  readArc(arcFile);
  return (MappingProblem*)this;
}

RemappingProblem* MappingProblemReader::read(char appFile[256], char proFile[256], char arcFile[256], char mapFile[256])
{
  vFaultyNodes = NULL; // vFaultyNodes is set afterwards explicitly. (according to the command line parameters)
  mpType = LIMITED_REMAPPING_PROBLEM;
  readApp(appFile);
  readPro(proFile);
  readArc(arcFile);
  readMap(mapFile);
  return (RemappingProblem*)this;
}

RAMappingProblem* MappingProblemReader::read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType)
{
  vFaultyNodes = NULL; // vFaultyNodes is set afterwards explicitly. (according to the command line parameters)
  mpType = RA_MAPPING_PROBLEM;
  ftType = _ftType;
  readApp(appFile);
  readPro(proFile);
  readArc(arcFile);
  readFR(frFile);

  // initialize Mtrt. Every task is a redundant instance of itself.
  if (ftType == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS)
    {
      for(int i=0; i<T(); i++)
	{
	  vector<int> tmp;
	  tmp.push_back(i);
	  Mtrt[i] = tmp;
	}
    }

  return (RAMappingProblem*)this;
}

RAMappingProblem* MappingProblemReader::read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType, double _compTimeThreshold)
{
  mpType = RA_TC_MAPPING_PROBLEM;
  compTimeThreshold = _compTimeThreshold;
  ftType = _ftType;
  readApp(appFile);
  readPro(proFile);
  readArc(arcFile);
  readFR(frFile);
  return (RAMappingProblem*)this;
}

RAMappingProblem* MappingProblemReader::read(char appFile[256], char proFile[256], char arcFile[256], char frFile[256], FaultTreeType _ftType, char trtFile[256])
{
  vFaultyNodes = NULL; // vFaultyNodes is set afterwards explicitly. (according to the command line parameters)
  mpType = RA_MAPPING_PROBLEM;
  ftType = _ftType;
  readApp(appFile);
  readPro(proFile);
  readArc(arcFile);
  readFR(frFile);

  // read Mtrt
  if (ftType == NO_RECONFIGURATION_WITH_REDUNDANT_TASKS)
    {
		readTRT(trtFile);
    }

  return (RAMappingProblem*)this;
}

int* MappingProblemReader::coord(int node_num)
{
	int* coord_XY = (int*)malloc(2);

	coord_XY[0]= (int) ((node_num-1)/sqrt((double)_N)); // y coordinate
	coord_XY[1]= (int) ((node_num-1)-((coord_XY[0])* sqrt((double)_N))); // x coordinate
	

	return &(coord_XY[0]);
}

void MappingProblemReader::find(int coordinate1, int coordinate2, int tag, Link linksXdirection[], int pathnum){

	for(int i=0;i<_L/2;i++){
		if(linksXdirection[i].coord1==coordinate1 && linksXdirection[i].coord2==coordinate2 && linksXdirection[i].tag == tag || linksXdirection[i].coord1==coordinate2 && linksXdirection[i].coord2==coordinate1 && linksXdirection[i].tag == tag){
		//	printf("%d\n",linksXdirection[i].linknum);
			_Mpl[pathnum][linksXdirection[i].linknum]=1;
			break;
		}
	}
}

void MappingProblemReader::path(int coord1, int coord2, int pathnum){

	int* coordinatenode1;
	int* coordinatenode2;
	 

	/******* finding the links in the path *******/
	
	coordinatenode1=coord(coord1);
	coordinatenode2=coord(coord2);

//	printf("%d,%d\n",coordinatenode1[0],coordinatenode1[1]);
//	printf("%d,%d\n",coordinatenode2[0],coordinatenode2[1]);

	if(coordinatenode2[1]>coordinatenode1[1]){	//move in X direction
		for(int i=coordinatenode1[1];i<coordinatenode2[1];i++){
			find(i,i+1,coordinatenode1[0],linksXdirection,pathnum);
		}
	}

	else if(coordinatenode1[1]>coordinatenode2[1]){	//move in X direction
		for(int i=coordinatenode2[1];i<coordinatenode1[1];i++){
			find(i,i+1,coordinatenode1[0],linksXdirection,pathnum);
		}
	}
	else{
		
	}

	coordinatenode1[1]=coordinatenode2[1];

	if(coordinatenode2[0]>coordinatenode1[0]){	//move in Y direction
		for(int i=coordinatenode1[0];i<coordinatenode2[0];i++){
			find(i,i+1,coordinatenode1[1],linksYdirection,pathnum);
		}
	}

	else if(coordinatenode1[0]>coordinatenode2[0]){	//move in Y direction
		for(int i=coordinatenode2[0];i<coordinatenode1[0];i++){
			find(i,i+1,coordinatenode1[1],linksYdirection,pathnum);
		}
	}
	else{
		
	}

	free(coordinatenode1);
	free(coordinatenode2);
}

void MappingProblemReader::generateArchitectureMatrices(){

	int temp_N=0;
	int temp=0;
	int j; // onemli boyle olmali

	temp_N=_N;
	_P = _N * (_N-1);
	//printf("%d\n",P);
	_L = (int) 2*sqrt((double) _N)*(sqrt((double) _N)-1);
	//printf("%d\n",L);

	/******* _Mpn memory allocation and initialization *******/ 
	
	_Mpn = (int **) calloc( _P, sizeof(int*));

	for(int i = 0; i < _P; ++i) {
		_Mpn[i] = (int *) calloc(_N, sizeof(int));
	}
	
	/******* _Mpl memory allocation and initialization *******/ 
	
	_Mpl = (int **) calloc( _P, sizeof(int*));

	for(int i = 0; i < _P; ++i) {
		_Mpl[i] = (int *) calloc(_L, sizeof(int));
	}

	/******* _Mnc memory allocation and initialization *******/
	/* This is read from the arc file. Not needed anymore.
	_Mnc = (int **) calloc( _N, sizeof(int*));

	for(int i = 0; i < _N; ++i) {
		_Mnc[i] = (int *) calloc(_CT, sizeof(int));
	}*/

	/******* links memory allocation and initialization *******/

	linksXdirection = (Link *) calloc( _L/2, sizeof(Link));
	linksYdirection = (Link *) calloc( _L/2, sizeof(Link));
	for(int i=0;i<(sqrt((double)_N));i++){
		for(int j=0;j<(sqrt((double)_N)-1);j++){
			linksXdirection[(i*((int)sqrt((double)_N)-1))+j].tag=i;
			linksXdirection[(i*((int)sqrt((double)_N)-1))+j].coord1=j;
			linksXdirection[(i*((int)sqrt((double)_N)-1))+j].coord2=j+1;
			linksXdirection[(i*((int)sqrt((double)_N)-1))+j].linknum=i*(sqrt((double)_N)-1)+j;
		}
	}

	for(int i=0;i<(sqrt((double)_N));i++){
		for(int j=0;j<(sqrt((double)_N)-1);j++){
			linksYdirection[(i*((int)sqrt((double)_N)-1))+j].tag=i;
			linksYdirection[(i*((int)sqrt((double)_N)-1))+j].coord1=j;
			linksYdirection[(i*((int)sqrt((double)_N)-1))+j].coord2=j+1;
			linksYdirection[(i*((int)sqrt((double)_N)-1))+j].linknum=i*(sqrt((double)_N)-1)+j+_L/2;
		}
	}

/*	for(int i=0;i<L/2;i++){
		printf("%d,%d,%d,%d\n",linksXdirection[i].coord1,linksXdirection[i].coord2,linksXdirection[i].linknum,linksXdirection[i].tag);
	} 

	for(int i=0;i<L/2;i++){
		printf("%d,%d,%d,%d\n",linksYdirection[i].coord1,linksYdirection[i].coord2,linksYdirection[i].linknum,linksYdirection[i].tag);
	} */


	/******* _Mpn & _Mpl calculation *******/ 

	for(int i=0;i<_N;i++){
		for(j=0;j<(temp_N-1);j++){
			
			_Mpn[j+temp][i]=1;
			_Mpn[j+temp+(_P/2)][i]=-1;

			_Mpn[j+temp][j+1+i]=-1;
			_Mpn[j+temp+(_P/2)][j+1+i]=1;

			path(i+1,j+2+i,j+temp);
	//		printf("\n");
			path(j+2+i,i+1,j+temp+(_P/2));
	//		printf("\n");

		}
		temp_N--;
		temp+=j;

	}


/** BU KISIM COMMENT'LI OLMALIYDI. TC'nin ilk submission'indan sonra farkettim.
	\/******* _Mnc calculation *******\/
	for(int i=0; i < _N ;i++){
			_Mnc[i][(i%_CT)]=1;
	}
*/

/*	for(int i=0;i<P;i++){	//	PRINT _Mpl[P][L]
		for(int j=0;j<L;j++){
			printf("%d,",_Mpl[i][j]);
		}
		printf("\n");
	} */

/*	for(int i=0;i<N;i++){	//	PRINT _Mnc[N][_CT]
		for(int j=0;j<_CT;j++){
			printf("%d,",_Mnc[i][j]);
		}
		printf("\n");
	} */

/*	for(int i=0;i<P;i++){	//	PRINT _Mpn[P][N]
		for(int j=0;j<N;j++){
			printf("%d,",_Mpn[i][j]);
		}
		printf("\n");
	} */


}

/*
int MappingProblemReader::generate(char TGfile[256], char TCfile[256]){

	read(TGfile, TCfile);
	ArchitectureMatrices();

	// link bandwidth "C" may be asked to the user or defined directly.

	//	_getch();

	return 0;
}
*/

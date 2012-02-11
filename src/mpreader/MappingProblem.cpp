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
#include "MappingProblem.h"

MappingProblem::MappingProblem()
{
  namePrefix = "";
}

string MappingProblem::name()
{
  std::stringstream _str;
  _str << namePrefix << "-N" << N() << "-T" << T();
  if (vFaultyNodes != NULL)
	_str << "-" << getFaultScenarioString();
  return _str.str();
}

string MappingProblem::getFaultScenarioString()
{
  std::stringstream _str;
  if (vFaultyNodes != NULL)
  {
	_str << "faulty";
	for (vector<int>::iterator it = vFaultyNodes->begin(); it != vFaultyNodes->end(); it++)
		_str << "-n" << *it;
  }
  else
	_str << "no_fault";

  return _str.str();
}

void MappingProblem::setNamePrefix(string _prefixStr)
{
  this->namePrefix = _prefixStr;
}

void MappingProblem::write(string filename)
{
  ofstream fout1( (filename+".app").c_str() );
  ofstream fout2( (filename+".pro").c_str() );
  ofstream fout3( (filename+".arc").c_str() );

  // write app file.
  fout1 << T() << endl
	<< E() << endl
	<< endl;
  for(int j=0;j<E();j++)
    {
      int src, dst;
      for(int i=0;i<T();i++)
	{
	  if(Mte()[i][j] == 1)
	    src = i;
	  if(Mte()[i][j] == -1)
	    dst = i;
	}
      // in app file tasks' indices start from 1
      fout1 << src+1 << "\t" << dst+1 << "\t" << d()[j] << endl;
    }
  fout1.close();

  // write pro file.
  fout2 << endl
	<< CT() << endl
	<< endl;
  for(int i=0; i<T(); i++)
    {
      for(int j=0; j<CT(); j++)
	{
	  fout2 << Ttc()[i][j] << "\t";
	}
      fout2 << endl;
    }
  fout2.close();

  // write arc file.
  fout3 << N() << endl
	<< CT() << endl
	<< C() << endl
	<< endl;

  for(int i=0; i<N(); i++)
    {
      for(int j=0; j<CT(); j++)
	fout3 << Mnc()[i][j] << "\t";
      fout3 << endl;
    }

}

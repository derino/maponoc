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
#include "RAMappingProblem.h"

bool RAMappingProblem::isForkTask(int t)
{
  for( map<int, vector<int> >::iterator it = Mtrt.begin(); it != Mtrt.end(); it++)
    {
      if( it->first == FORK_KEY)
	{
	  for(vector<int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
	    if(*vit == t)
	      return true;
	}
    }
  return false;
}

bool RAMappingProblem::isVoterTask(int t)
{
  for( map<int, vector<int> >::iterator it = Mtrt.begin(); it != Mtrt.end(); it++)
    {
      if( it->first == VOTER_KEY)
	{
	  for(vector<int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
	    if(*vit == t)
	      return true;
	}
    }
  return false;
}

// e.g., if i is id of t2_2, returns int ids of t2_1, t2_2, t2_3
vector<int> RAMappingProblem::redundantsOfTask(int i)
{
  map<int, vector<int> >::iterator mit;
  for(mit = Mtrt.begin(); mit != Mtrt.end(); mit++)
    {
      for(vector<int>::iterator rit = mit->second.begin(); rit != mit->second.end(); rit++)
	{
	  if(*rit == i)
	    return mit->second;
	}
    }
    assert (false); // shouldn't ever reach here.
    return mit->second;
}

// writes Mtrt in addition to the ones of MappingProblem::write
void RAMappingProblem::write(string filename)
{
  //  ((MappingProblem*)this)->write(filename);
  this->MappingProblem::write(filename);

  ofstream fout( (filename+".trt").c_str() );
  
  //fout.open ( (filename+".trt").c_str() , fstream::app);

  for( map<int, vector<int> >::iterator it = Mtrt.begin(); it != Mtrt.end(); it++)
    {
      fout << it->first << ": ";
      for( vector<int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
	{
	  fout << *vit << " ";
	}
      fout << endl;
    }
	 fout.close();
}

void RAMappingProblem::readTRT(char trtFile[256])
{
  fstream fin(trtFile);
  if ( !fin.is_open())
    {
      cout << "Unable to open trt_file"; 
      exit(EXIT_FAILURE);
    }
    
//   char line[1024];
//   while( !fin.getline(line, 1024).eof() )
	string line;
   while( !getline(fin, line).eof() )
     {
//       cout << "line=" << line << endl;
       char* linec = new char[line.length()];
       strcpy(linec, line.c_str());
       char* pch;
       pch = strtok (linec,": ");
       int tOrig = atoi(pch);
       //cout << "tOrig:" << tOrig << endl;
       pch = strtok (NULL," ");
       while (pch != NULL)
         {
           //cout << "pch=" << pch << endl;
           int tR = atoi(pch);
           //cout << "tR:" << tR;
           Mtrt[tOrig].push_back(tR);
           pch = strtok (NULL, " ");
         }
         //cout << endl << endl;
     }
}


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
#include "MappingMOMHSolution.hpp"

extern MappingProblem* theMP;
//extern ReliabilityCalculator* theRC;

MappingMOMHSolution::MappingMOMHSolution() : TDPAMConstrainedSolution()/*TMOMHSolution()*/, MappingSolution()
{// DONE: create a random mapping solution using a global mapping problem variable
  //cout << "const." << endl;

  mp = theMP;
  status = FEASIBLE_SOLUTION;
  
  // allocate Xnt
  Xnt = new int*[mp->N()];
  for (int i=0; i< mp->N(); i++)
    Xnt[i] = new int[mp->T()];
  
  // assign each task on a random node
  for (int i=0; i< mp->T(); i++)
    {
      int nodeIndex;

      // special treatment for the reliability aware mapping problem
      if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
	{
	  RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
	  vector<int> v = rmp->getForkVoterNodes();
	  // if it's a fork or voter task, then assign it already on a forkvoter core.
	  if( rmp->isForkTask(i) || rmp->isVoterTask(i) )
	      nodeIndex = v[rand() % v.size()]; // choose one of the forkvoter nodes randomly
	  else 
	    // don't map a normal task on the forkvoter nodes
	    // don't map redundant instances of the same task type on the same node
	    {
	      //bool redundantsOnTheSameNode;
	      //vector<int> vRT_i = rmp->redundantsOfTask(i); // e.g., if i is t2_2, returns int ids of t2_1, t2_2, t2_3
	      do
	      {
		  nodeIndex = rand() % mp->N();  
		  //redundantsOnTheSameNode = false;

		  // check the mappings of redundant tasks of the same task type up to task i to see if there is a redundant instance on the same node
		  
		  //for(vector<int>::iterator it = vRT_i.begin(); it != vRT_i.end(); it++)
		  //{
		  //if(*it < i && node(*it) == nodeIndex )
		  //{
		  //redundantsOnTheSameNode = true;
		  //break;
		  //}
		  //}
		  //		  cout << "t:" << i << " n:" << nodeIndex << endl;
		}
	      while( rmp->isForkVoterNode(nodeIndex) /*|| redundantsOnTheSameNode*/ );
	    }
	}
      else
	{
	  nodeIndex = rand() % mp->N();  
	}
      
      for(int j=0; j<mp->N(); j++)
	{
	  Xnt[j][i] = (j==nodeIndex? 1: 0);
	}
    } // END for each task


  // don't map redundant instances of the same task type on the same node
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
      fixRedundantsMapping(rmp);
    }

  /*
  cout << "initial:" << endl;
  for(int j=0; j<mp->N(); j++)
    {
      for(int i=0; i<mp->T(); i++)
	{
	  cout << Xnt[j][i] << " ";
	}
      cout << endl;
    }
  cout << endl;
  cout << endl;
  */

  // allocate Ype
  Ype = new int*[mp->P()];
  for (int i=0; i< mp->P(); i++)
    Ype[i] = new int[mp->E()];
  
  // initialize Ype according to Xnt
  initializeYpe();
  
  // calculate objective values
  if( mp->mpType == MAPPING_PROBLEM )
    {
      ObjectiveValues.resize(2);
      ObjectiveValues[0] = getCompCost();
      ObjectiveValues[1] = getCommCost();
    }
  else if( mp->mpType == RA_MAPPING_PROBLEM )
    {
      ObjectiveValues.resize(3);
      ObjectiveValues[0] = getCompCost();
      ObjectiveValues[1] = getCommCost();
      ObjectiveValues[2] = getMTTF();
    }
  else if( mp->mpType == RA_TC_MAPPING_PROBLEM )
    {
      ObjectiveValues.resize(3);
      ObjectiveValues[0] = getCompCost(); // this is inactive for RA_TC
      ObjectiveValues[1] = getCommCost();
      ObjectiveValues[2] = getMTTF();
    }
  else
    {
      cout << "Error: GAMapper is used with an unsupported mapping problem." << endl;
      exit(EXIT_FAILURE);
    }

  //constraint values
  ConstraintValues.resize(mp->L());
  // calculate constraint values: Mlp Ype d < c
  calculateConstraintValues();

  if( mp->mpType == RA_TC_MAPPING_PROBLEM )
    {
      ConstraintValues.resize(mp->L() + 1);
      ConstraintValues[mp->L()] = ObjectiveValues[0];  
    }
  //print(cout);
  //cout << *this << endl;
}

void MappingMOMHSolution::calculateConstraintValues()
{
  int** Mle = new int*[mp->L()];
  for (int i=0; i<mp->L(); i++)
    Mle[i] = new int[mp->E()];

  for (int i = 0; i<mp->L(); i++)
    for(int j = 0; j<mp->E(); j++)
      {
	Mle[i][j] = 0;
	for (int k = 0; k<mp->P(); k++)
	  {
	    Mle[i][j] += mp->Mpl()[k][i]*Ype[k][j];
	  }
      }

  // Mle * d
  for (int i=0; i<mp->L(); i++)
    {
      ConstraintValues[i] = 0;  
      for (int j=0; j<mp->E(); j++)
	ConstraintValues[i] += Mle[i][j]*mp->d()[j];
    }

  // delete Mle
  for (int i=0; i<mp->L(); i++)
    delete[] Mle[i];
  delete[] Mle;
}

void MappingMOMHSolution::initializeYpe()
{
  // first set all to zero
  for(int i=0; i<mp->P(); i++)
    for(int j=0; j<mp->E(); j++)
	Ype[i][j] = 0;

  // calculate Mne
  int** Mne = new int*[mp->N()];
  for (int i=0; i<mp->N(); i++)
    Mne[i] = new int[mp->E()];

  for(int i=0; i<mp->N(); i++)
    for(int j=0; j<mp->E(); j++)
      {
	Mne[i][j] = 0;
	for(int k=0; k<mp->T(); k++)
	  Mne[i][j] += Xnt[i][k]*mp->Mte()[k][j];
      }

  // for(int i=0; i<mp->N(); i++)
  //   {
  //     for(int j=0; j<mp->E(); j++)
  // 	{
  // 	  cout << Mne[i][j] << " ";
  // 	}
  //     cout << endl;
      
  //   }
  


  // calculate Ype: Mne = Mnp Ype
  // for each data dependency Ei, find the source(srcN) and destination node (dstN) of the data dependency (acc. to the given mapping, Xnt). Using Mpn, find the path (Pk) corresponding to the path between srcN and dstN. set Ype[k][i] = 1.
  for (int i=0; i<mp->E(); i++)
    {
      // find the source(srcN) and destination node (dstN) of the data dependency (acc. to the given mapping, Xnt)
      int srcN=-1, dstN=-1;

      for (int j=0; j<mp->N(); j++)
	{
	  if(Mne[j][i] == 1)
	    srcN = j;
	  else if(Mne[j][i] == -1)
	    dstN = j;
	}
      //TODO: assert srcN != -1 && dstN != -1

      // Using Mpn, find the path (Pk) corresponding to the path between srcN and dstN.
      if (dstN != -1) // the data dependecy is between task that are mapped on the same node
	{
	  int k;
	  for (k=0; k<mp->P(); k++)
	    {
	      if(mp->Mpn()[k][srcN] == 1 && mp->Mpn()[k][dstN] == -1)
		break;
	    }
	  
	  Ype[k][i] = 1;
	}
      
    } // /END for data dependency i

  // delete Mne
  for (int i=0; i<mp->N(); i++)
    delete[] Mne[i];
  delete[] Mne;

}

/*MappingMOMHSolution::MappingMOMHSolution(MappingProblem* _mp) : TMOMHSolution(), MappingSolution(_mp)
{// TODO: create a random mapping solution

}*/

MappingMOMHSolution::MappingMOMHSolution(MappingMOMHSolution& mappingMOMHSolution) : TDPAMConstrainedSolution/*TMOMHSolution*/(mappingMOMHSolution), MappingSolution(mappingMOMHSolution)
{// copy-constructor for MappingSolution

  mp = mappingMOMHSolution.getMP();
  status = mappingMOMHSolution.getStatus();

  // allocate Xnt  
  Xnt = new int*[mp->N()];
  for (int i=0; i< mp->N(); i++)
    Xnt[i] = new int[mp->T()];
  
  // allocate Ype
  Ype = new int*[mp->P()];
  for (int i=0; i< mp->P(); i++)
    Ype[i] = new int[mp->E()];

  // copy Xnt
  for (int i=0; i< mp->N(); i++)
    for (int j=0; j< mp->T(); j++)
      Xnt[i][j] = mappingMOMHSolution.getXnt()[i][j];
  
  // copy Ype
  for (int i=0; i< mp->P(); i++)
    for (int j=0; j< mp->E(); j++)
      Ype[i][j] = mappingMOMHSolution.getYpe()[i][j];

  
  ObjectiveValues[0] = mappingMOMHSolution.ObjectiveValues[0]; // mappingMOMHSolution.getCompCost();
  ObjectiveValues[1] = mappingMOMHSolution.ObjectiveValues[1]; //mappingMOMHSolution.getCommCost();

  if( mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      ObjectiveValues[2] = mappingMOMHSolution.ObjectiveValues[2];
      // ObjectiveValues[2] = getMTTF();
    }

  ConstraintValues = mappingMOMHSolution.ConstraintValues; // copies all
  //print(cout);
}

MappingMOMHSolution::MappingMOMHSolution(MappingMOMHSolution& parent1, MappingMOMHSolution& parent2) : TDPAMConstrainedSolution/*TMOMHSolution*/(parent1, parent2), MappingSolution()
{// TODO: Define copy-constructor for MappingSolution
  //cout << "cross-over const." << endl;


  //TODO: assert parent1 and parent2 belongs to the same problem.
  mp = parent1.getMP(); //theMP;
  status = FEASIBLE_SOLUTION;

// allocate Xnt  
  Xnt = new int*[mp->N()];
  for (int i=0; i< mp->N(); i++)
    Xnt[i] = new int[mp->T()];
  
  // allocate Ype
  Ype = new int*[mp->P()];
  for (int i=0; i< mp->P(); i++)
    Ype[i] = new int[mp->E()];
  
  // cross-over is defined by getting the nodes of a random number of tasks from parent1 and the rest from parent2.

  // random crossover point between [1,T-1] so that not all genes are from only one parent
  int crossOverPoint = rand() % (mp->T()-1) + 1;
  // from parent1
  for (int i=0; i<crossOverPoint; i++)
    {
      for (int j=0; j<mp->N(); j++)
	{
	  Xnt[j][i] = parent1.getXnt()[j][i];
	}
    }
  // from parent2
  for (int i=crossOverPoint; i<mp->T(); i++)
    {
      for (int j=0; j<mp->N(); j++)
	{
	  int** p2Xnt = parent2.getXnt();
	  if(p2Xnt == NULL) cerr << "NULL burda" << endl;
	  Xnt[j][i] = p2Xnt[j][i];
	}
    }

  // if RA mapping problem, then make sure redundants of same type don't end up on the same node
  // after the cross-over
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
      fixRedundantsMapping(rmp);
    }

  // initialize Ype according to Xnt
  initializeYpe();

  // calculate objective values
  ObjectiveValues[0] = getCompCost();
  ObjectiveValues[1] = getCommCost();
  if( mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    ObjectiveValues[2] = getMTTF();

  // constraints
  ConstraintValues.resize(parent1.ConstraintValues.size());
  if( mp->mpType == RA_TC_MAPPING_PROBLEM )
    {
      ConstraintValues[mp->L()] = ObjectiveValues[0];  
    }
  // calculate constraint values: Mlp Ype d < c
  calculateConstraintValues();

  //  cout << *this << endl;
  //print(cout);
  /*  cout << "after crossover:" << endl;
  for(int j=0; j<mp->N(); j++)
    {
      for(int i=0; i<mp->T(); i++)
	{
	  cout << Xnt[j][i] << " ";
	}
      cout << endl;
    }
  cout << endl;
  cout << endl;
  */
}

void MappingMOMHSolution::Mutate()
{
  //  Mutate_MoveTaskToNeighborNode();
  // choose the mutation strategy at random
    
  int mutationStrategy = rand() % 2;

  switch(mutationStrategy)
    {
    case 0:
      Mutate_MoveTaskToNeighborNode();
      break;
    case 1:
      Mutate_UnitePeers();
      break;
    case 2:
      Mutate_BalanceLoad();
      break;
    default:
      cout << "Error: This should not ever print!" << endl;
    }
  

  // after mutation, if redundant instances of the same task type end up on the same node
  // move them away randomly
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
      fixRedundantsMapping(rmp);
    }

  // reinitialize Ype
  initializeYpe();

  // calculate objective values
  ObjectiveValues[0] = getCompCost();
  ObjectiveValues[1] = getCommCost();

  if( mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    ObjectiveValues[2] = getMTTF();

  // constraints
  // calculate constraint values: Mlp Ype d < c
  calculateConstraintValues();

  if( mp->mpType == RA_TC_MAPPING_PROBLEM )
    {
      ConstraintValues[mp->L()] = ObjectiveValues[0];  
    }

  //  cout << *this << endl;
  /*  cout << "after mutate:" << endl;
  for(int j=0; j<mp->N(); j++)
    {
      for(int i=0; i<mp->T(); i++)
	{
	  cout << Xnt[j][i] << " ";
	}
      cout << endl;
    }
  cout << endl;
  cout << endl;
  */
}

/** TODO */
void MappingMOMHSolution::Mutate_BalanceLoad()
{
}

void MappingMOMHSolution::Mutate_UnitePeers()
{
  // choose a random task, t_i, and bring all its peer tasks to the same node as t_i, n_j.
  int t_i;
  int n_j = -1;

  // in case of RA problem, exclude fork or voter tasks from mutation
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
      do
	t_i = rand() % mp->T();
      while( rmp->isForkTask(t_i) || rmp->isVoterTask(t_i) );
    }
  else
    t_i = rand() % mp->T();



  for (int j=0; j < mp->N(); j++)
    {
      if(Xnt[j][t_i] == 1)
	{
	  n_j = j;
	  break;
	}
    }
  assert(n_j != -1); // t_i should be mapped on a node.

  // migrate peer tasks to n_j
  for(int i=0; i<mp->E(); i++)
    {
      int tSrc = -1;
      int tDst = -1;
      for(int j=0; j<mp->T(); j++)
	{
	  if( mp->Mte()[j][i] == 1 )
	    tSrc = j;
	  else if( mp->Mte()[j][i] == -1 )
	    tDst = j;
	}
      assert(tSrc != -1 || tDst != -1); // an edge should have src and dst tasks


      // if peers are fork or voter tasks, don't move them
      if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
	{
	  RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
	  if (t_i == tSrc && (!rmp->isForkTask(tDst) && !rmp->isVoterTask(tDst)) )
	    migrate(tDst, n_j);
	  else if(t_i == tDst && (!rmp->isForkTask(tSrc) && !rmp->isVoterTask(tSrc)) )
	    migrate(tSrc, n_j);
	}
      else
	{
	  if (t_i == tSrc)
	    migrate(tDst, n_j);
	  else if(t_i == tDst)
	    migrate(tSrc, n_j);
	}
    } // END for each peer task
}

// change task t's mapping to newNode
void MappingMOMHSolution::migrate(int t, int newNode)
{
  for (int i=0; i<mp->N(); i++)
    {
      if(Xnt[i][t] == 1)
	Xnt[i][t] = 0;
    }
  Xnt[newNode][t] = 1;
}

void MappingMOMHSolution::Mutate_MoveTaskToNeighborNode()
{
  // choose a random task and move it to a neighbour node.
  // more specifically, choose t_i, find n_j = map(t_i).
  // for all paths that involve n_j (using Mpn), find the paths
  // with length 1 (using Mpl) and find the nodes that are 
  // 1 length away. randomly choose a new node among them.
  // change t_i's mapping and reinitialize Ype.
  //cout << "mutate" << endl;


  int t_i;

  // in case of RA problem, exclude fork or voter tasks from mutation
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);
      do
	t_i = rand() % mp->T();
      while( rmp->isForkTask(t_i) || rmp->isVoterTask(t_i) );
    }
  else
    t_i = rand() % mp->T();
  


  int n_j = -1;
  for (n_j=0; n_j<mp->N(); n_j++)
    {
      if(Xnt[n_j][t_i] == 1)
	break;
    }
  assert(n_j != -1); // t_i should be mapped on a node.


  // paths of length 1 that involve n_j
  vector<int> pathsNj;// = new vector<int>();
  for(int i=0; i<mp->P(); i++)
    {
      for(int j=0; j<mp->N(); j++)
	{
	  if(j==n_j && (mp->Mpn()[i][j] == 1 /*|| mp->Mpn()[i][j] == -1*/) ) // it's enough to check only for the cases in which n_j is the source to avoid duplicates.
	    {
	      // for path_i, calculate the length and add to pathsNj if length == 1.
	      int len = 0;
	      for(int k=0; k<mp->L(); k++)
		if(mp->Mpl()[i][k] == 1)
		  len++;

	      if(len == 1)
		pathsNj.push_back(i); 
	    } 
	}
    }
  
  // select randomly a path of length one from n_j and find the node (nNew) at the other end of the path
  int nNew;

  // if RA problem, nNew can not be a forkvoter node
  if(mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM)
    {
      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*> (mp);

      int randIndex = rand() % pathsNj.size();
      int p = pathsNj[randIndex];
      // find the sink node of the path p
      for(nNew=0; nNew<mp->N(); nNew++)
	if(mp->Mpn()[p][nNew] == -1)
	  break;

      // if nNew is a forkvoter node, don't move.
      if (rmp->isForkVoterNode(nNew) )
	nNew = n_j;
    }
  else
    {
      int randIndex = rand() % pathsNj.size();
      int p = pathsNj[randIndex];
      // find the sink node of the path p
      for(nNew=0; nNew<mp->N(); nNew++)
	if(mp->Mpn()[p][nNew] == -1)
	  break;
    }

  //  cout << "moving " << t_i << " from " << n_j << " to " << nNew << endl;

  // move t_i to nNew
  Xnt[n_j][t_i] = 0;
  Xnt[nNew][t_i] = 1;

}

double MappingMOMHSolution::getMTTF()
{
  RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*>(mp);
  ReliabilityCalculator rc;
  rcInstance = &rc;
  return rc.calculateMTTF(rmp, this);
}

std::ostream& operator<<(std::ostream& os, MappingMOMHSolution& ms)
{
//DONE: if RAMappingProblem, print also the MTTF value.
  os << ms.getCompCost() << "\t" << ms.getCommCost();
  if( ms.getMP()->mpType == RA_MAPPING_PROBLEM || ms.getMP()->mpType == RA_TC_MAPPING_PROBLEM)
    os << "\t" << setprecision(18) << ms.getMTTF();
  return os;
}

void MappingMOMHSolution::print(ostream& out)
{
  //out << "Solution status = " << status << endl;
  if ( Feasible() ) //(status == OPTIMAL_SOLUTION/*IloAlgorithm::Optimal*/ || status == FEASIBLE_SOLUTION)
    {
      out << "Comm. cost  = " << getCommCost() << endl;
      out << "Comp. cost  = " << getCompCost() << endl;
      if( mp->mpType == RA_MAPPING_PROBLEM || mp->mpType == RA_TC_MAPPING_PROBLEM )
	out << "MTTF  = " << getMTTF() << endl;
            
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

      RAMappingProblem* rmp = dynamic_cast<RAMappingProblem*>(mp);
      ReliabilityCalculator rc;
      rcInstance = &rc;
      rc.printFaultTree(rmp, this);

    } // end if feasible
}

// if redundant tasks of same task type are on the same node, move them to random nodes such that
// they are not anymore.
// It is called only if it's a RA mapping problem and by initial, cross-over and mutation
void MappingMOMHSolution::fixRedundantsMapping(RAMappingProblem* rmp)
{
  for (int i=0; i< mp->T(); i++)
    {
      if( rmp->isForkTask(i) || rmp->isVoterTask(i) )
	continue;
      else 	// don't map redundant instances of the same task type on the same node
	{
	  vector<int> vRT_i = rmp->redundantsOfTask(i); // e.g., if i is t2_2, returns int ids of t2_1, t2_2, t2_3
		  
	  for(vector<int>::iterator it = vRT_i.begin(); it != vRT_i.end(); it++)
	    {
	      if(*it > i && node(*it) == node(i) )
		{
		  int nodeIndex;
		  do
		    {
		      nodeIndex = rand() % mp->N();  
		    }
		  while( rmp->isForkVoterNode(nodeIndex) || nodeIndex == node(i) );

		  for(int j=0; j<mp->N(); j++)
		    {
		      Xnt[j][*it] = (j==nodeIndex? 1: 0);
		    }
		}
	    }

	}
    } // END for each task
}

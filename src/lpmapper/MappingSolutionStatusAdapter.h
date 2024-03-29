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
#ifndef MAPPING_SOLUTION_STATUS_ADAPTER_H
#define MAPPING_SOLUTION_STATUS_ADAPTER_H

#include "MappingSolution.h"

static MappingSolutionStatus mappingSolutionStatusAdapter(IloAlgorithm::Status ias)
{
  MappingSolutionStatus status = UNKNOWN_SOLUTION;

  switch(ias)
    {
    case IloAlgorithm::Feasible:
      status = FEASIBLE_SOLUTION;
      break;
    case IloAlgorithm::Optimal:
      status = OPTIMAL_SOLUTION;
      break;
    case IloAlgorithm::Infeasible:
      status = INFEASIBLE_SOLUTION;
      break;
    case IloAlgorithm::Unbounded:
      status = UNBOUNDED_SOLUTION;
      break;
    case IloAlgorithm::InfeasibleOrUnbounded:
      status = INFEASIBLE_OR_UNBOUNDED_SOLUTION;
      break;
    case IloAlgorithm::Error:
      status = ERROR_SOLUTION;
      break;
    default:
      status = UNKNOWN_SOLUTION;
      break;
    }

  return status;

  //  if(ia.getStatus() == IloAlgorithm::UNKNOWN)
  //  status = UNKNOWN_SOLUTION;
  //else

}

/* class IloAlgorithmAdapter */
/* { */
/*  private: */
/*   MappingSolutionStatus status; */
  
/*  public: */
/*   IloAlgorithmAdapter(IloAlgorithm ia); */
/*   MappingSolutionStatus getStatus(); */
/* } */

#endif

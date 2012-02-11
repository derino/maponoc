/*
Multiple Objective MetaHeuristics Library in C++ MOMHLib++
Copyright (C) 2001 Andrzej Jaszkiewicz

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation (www.gnu.org); 
either version 2.1 of the License, or (at your option) any later 
version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// momhconstrainedsolution.cpp: implementation of the TMOMHConstrainedSolution class.
//
//////////////////////////////////////////////////////////////////////

#include "momhconstrainedsolution.h"

bool TMOMHConstrainedSolution::ConstraintMet(int iConstraint)
{
    switch (Constraints[iConstraint].ConstraintType)
    {
      case _EqualTo:
          return fabs(ConstraintValues[iConstraint] - Constraints[iConstraint].Threshold) <
              Constraints[iConstraint].EqualityPrecision;
      case _LowerThan:
          return ConstraintValues[iConstraint] <= Constraints[iConstraint].Threshold;
      case _GreaterThan:
          return ConstraintValues[iConstraint] >= Constraints[iConstraint].Threshold;
      default:
          cout << "bool TMOMHConstrainedSolution::ConstraintMet (int iConstraint)\n";
          cout << "Unknown ConstraintType\n";
          exit(0);
    }
    return true;
}

double TMOMHConstrainedSolution::ConstraintViolation(int iConstraint)
{
    switch (Constraints[iConstraint].ConstraintType)
    {
      case _EqualTo:
          return fabs(ConstraintValues[iConstraint] - Constraints[iConstraint].Threshold);
      case _LowerThan:
          if (ConstraintValues[iConstraint] <= Constraints[iConstraint].Threshold)
              return 0;
          else
              return ConstraintValues[iConstraint] - Constraints[iConstraint].Threshold;
      case _GreaterThan:
          if (ConstraintValues[iConstraint] >= Constraints[iConstraint].Threshold)
              return 0;
          else
              return Constraints[iConstraint].Threshold - ConstraintValues[iConstraint];
      default:
          cout << "bool TMOMHConstrainedSolution::ConstraintViolation (int iConstraint)\n";
          cout << "Unknown ConstraintType\n";
          exit(0);
    }
    return 0;
}

bool TMOMHConstrainedSolution::Feasible()
{
    bool bFeasible = true;
    unsigned int iConstraint;
    for (iConstraint = 0; (iConstraint < NumberOfConstraints) && bFeasible; iConstraint++)
        bFeasible = ConstraintMet(iConstraint);
    return bFeasible;
}

double TMOMHConstrainedSolution::TotalConstraintsViolation()
{
    double TotalViolation = 0;
    unsigned int iConstraint;
    for (iConstraint = 0; iConstraint < NumberOfConstraints; iConstraint++)
        TotalViolation += ConstraintViolation(iConstraint);
    return TotalViolation;
}

/** OD */
TCompare TDPAMConstrainedSolution::Compare(TSolution & Solution)
{
    //cout << ">>tdpam::compare" << endl;
    TDPAMConstrainedSolution* downcastedSolution = dynamic_cast<TDPAMConstrainedSolution*>(&Solution);
    return this->ConstrainedCompare(*downcastedSolution);
}
/** /OD */

TCompare TDPAMConstrainedSolution::ConstrainedCompare(TDPAMConstrainedSolution & Solution)
{
    //cout << ">>>tdpam::constrainedcompare" << endl;
    //MappingMOMHSolution* downcastedSolution = dynamic_cast<MappingMOMHSolution*>(&Solution);
    
    cout.precision(dbl::digits10);
//    cout << "comparing (feasible:" << Feasible() << ", " << fixed << ObjectiveValues[0] << ", " << fixed << ObjectiveValues[1] << ", " << fixed << ObjectiveValues[2] << ")" << endl
//    << "with      " << "(feasible:" << Solution.Feasible() << ", " << fixed << Solution.ObjectiveValues[0] << ", " << fixed << Solution.ObjectiveValues[1] << ", " << fixed << Solution.ObjectiveValues[2] << ")";
    
    TCompare res;
    if (Feasible() && !Solution.Feasible())
        res = _Dominating;
    else if (!Feasible() && Solution.Feasible())
        res = _Dominated;
    else if (!Feasible() && !Solution.Feasible())
    {
        if (TotalConstraintsViolation() < Solution.TotalConstraintsViolation())
            res = _Dominating;
        else if (TotalConstraintsViolation() > Solution.TotalConstraintsViolation())
            res = _Dominated;
        else                    // TotalConstraintsViolation () == Solution.TotalConstraintsViolation ()
            res = ((TPoint*)this)->Compare(Solution); /** OD added prefix */
    } else                      // Feasible () && Solution.Feasible ())
        res = ((TPoint*)this)->Compare(Solution); /** OD added prefix */
        
        
    /*    
    switch(res)
    {
    	case _Dominating:
    	    cout << " \t DOMINATING" << endl;
    	    break;
    	case _Dominated:
    	    cout << " \t DOMINATED" << endl;
    	    break; 	
    	case _Nondominated:
    	    cout << " \t NONDOMINATED" << endl;
    	    break;
    	case _Equal:
    	    cout << " \t EQUAL" << endl;
    	    break;
    	default:
    	    cout << " \t ibnegibin:" << res << endl; 
    }
    cout << endl;
    */
    return res;       
}

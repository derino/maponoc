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

#include "global.h"

/** Number of objectives */
int NumberOfObjectives = 0;

/** Vector of objectives descriptions */
vector < TObjective > Objectives;

/** Number of constraints */
unsigned int NumberOfConstraints = 0;

/** Vector of constraints descriptions */
vector < TConstraint > Constraints;

/** Number of recombinations */
int NumberOfRecombinations;

const char *getMOMHName(MOMHType iType)
{
    switch (iType)
    {
      case MOMH_UNKNOWN:
          return "UNKNOWN";
      case MOMH_PSA:
          return "PSA";
      case MOMH_MOSA:
          return "MOSA";
      case MOMH_SMOSA:
          return "SMOSA";
      case MOMH_MOGLS:
          return "MOMGLS";
      case MOMH_IMMOGLS:
          return "IMMOGLS";
      case MOMH_PMA:
          return "PMA";
      case MOMH_MOMSLS:
          return "MOMSLS";
      case MOMH_SPEA:
          return "SPEA";
      case MOMH_NSGAII:
          return "NSGAII";
      case MOMH_NSGAIIC:
          return "NSGAIIC";
      case MOMH_NSGA:
          return "NSGA";
      default:
          return "UNKNOWN";
    }
}

const char *getMOMHGroupName(MOMHGroupType iType)
{
    switch (iType)
    {
      case MOMH_GROUP_UNKNOWN:
          return "UNKNOWN";
      case MOMH_GROUP_SA:
          return "SA";
      case MOMH_GROUP_PARETO:
          return "PARETO";
      case MOMH_GROUP_HGA:
          return "HGA";
      case MOMH_GROUP_MOMSLS:
          return "MOMSLS";
      default:
          return "UNKNOWN";
    }
}


MOMHGroupType getMOMHGroup(MOMHType iType)
{
    switch (iType)
    {
      case MOMH_UNKNOWN:
          return MOMH_GROUP_UNKNOWN;
      case MOMH_PSA:
          return MOMH_GROUP_SA;
      case MOMH_MOSA:
          return MOMH_GROUP_SA;
      case MOMH_SMOSA:
          return MOMH_GROUP_SA;
      case MOMH_MOGLS:
          return MOMH_GROUP_HGA;
      case MOMH_IMMOGLS:
          return MOMH_GROUP_HGA;
      case MOMH_PMA:
          return MOMH_GROUP_HGA;
      case MOMH_MOMSLS:
          return MOMH_GROUP_MOMSLS;
      case MOMH_SPEA:
          return MOMH_GROUP_PARETO;
      case MOMH_NSGAII:
          return MOMH_GROUP_PARETO;
      case MOMH_NSGAIIC:
          return MOMH_GROUP_PARETO;
      case MOMH_NSGA:
          return MOMH_GROUP_PARETO;
      default:
          return MOMH_GROUP_UNKNOWN;
    }
}

vector < MOMHType > getAllMOMHs()
{
    vector < MOMHType > result;

    result.clear();
    result.push_back(MOMH_PSA);
    result.push_back(MOMH_MOSA);
    result.push_back(MOMH_SMOSA);
    result.push_back(MOMH_MOGLS);
    result.push_back(MOMH_IMMOGLS);
    result.push_back(MOMH_PMA);
    result.push_back(MOMH_MOMSLS);
    result.push_back(MOMH_SPEA);
    result.push_back(MOMH_NSGAII);
    result.push_back(MOMH_NSGAIIC);
    result.push_back(MOMH_NSGA);

    return result;
}

MOMHType getMOMHTypeByName(const char *name)
{
    MOMHType type = MOMH_UNKNOWN;
    vector < MOMHType > allMOMHs = getAllMOMHs();

    for (unsigned int i = 0; i < allMOMHs.size(); i++)
    {
        if (!strcmp(getMOMHName(allMOMHs[i]), name))
        {
            type = allMOMHs[i];
        }
    }
    return type;
}

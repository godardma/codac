/** 
 *  codac2_Interval_operations.cpp
 *  
 *  This class reuses several functions developed for ibex::Interval. 
 *  The original IBEX code is encapsulated in Codac for allowing inheritance 
 *  to Codac classes and also for documentation, binding, and independency purposes.
 *  See ibex::Interval (IBEX lib, main author: Gilles Chabert)
 *    https://ibex-lib.readthedocs.io
 *
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Gilles Chabert, Simon Rohou
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#include <cassert>
#include "codac2_Interval_operations.h"

using namespace std;

namespace codac2
{
  Interval operator-(double x, const Interval& y)
  {
    return ibex::operator-(x, y);
  }

  Interval operator-(const Interval& x, double y)
  {
    return ibex::operator-(x, y);
  }
}
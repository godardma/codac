/** 
 *  codac2_CtcSegment.cpp
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Simon Rohou
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#include "codac2_CtcSegment.h"
#include "codac2_DirectedCtc.h"

using namespace std;
using namespace codac2;

std::shared_ptr<Ctc_<IntervalVector>> CtcSegment::copy() const
{
  return std::make_shared<CtcSegment>(*this);
}

void CtcSegment::contract(IntervalVector& x) const
{
  assert(x.size() == 2);
  IntervalVector a(_a), b(_b); // variables that may be contracted
  x &= a | b; // boxed domain of the segment
  IntervalVector c = a-x, d = b-a;
  DetOp::bwd(0.,d,c);
  SubOp::bwd(c[0],a[0],x[0]);
  SubOp::bwd(c[1],a[1],x[1]);
}
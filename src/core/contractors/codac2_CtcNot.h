/** 
 *  \file codac2_CtcNot.h
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Simon Rohou
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#pragma once

#include <map>
#include "codac2_CtcUnion.h"
#include "codac2_CtcInverse.h"

namespace codac2
{
  class CtcNot : public Ctc_<IntervalVector>
  {
    public:

      template<typename C, typename = typename std::enable_if<
          std::is_base_of_v<Ctc_<IntervalVector>,C> || std::is_same_v<std::shared_ptr<Ctc_<IntervalVector>>,C>
        >::type>
      CtcNot(const C& c)
        : Ctc_<IntervalVector>(size_of(c))
      { }

      std::shared_ptr<Ctc_<IntervalVector>> copy() const
      {
        return std::make_shared<CtcNot>(*this);
      }

      void contract(IntervalVector& x) const
      {
        /* does nothing: no inner information */
      }
  };
}
/* ============================================================================
 *  tubex-lib - CtcPicard class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#ifndef __TUBEX_CTCPICARD_H__
#define __TUBEX_CTCPICARD_H__

#include "tubex_Ctc.h"
#include "tubex_Fnc.h"
#include "tubex_Slice.h"
#include "tubex_Slice.h"

namespace tubex
{
  /**
   * \brief CtcPicard class.
   */
  class CtcPicard : public Ctc
  {
    public:

      CtcPicard(float delta = 1.1);
      void contract(const tubex::Fnc& f,
                    Tube& x,
                    TPropagation t_propa = FORWARD | BACKWARD);
      void contract(const tubex::Fnc& f,
                    TubeVector& x,
                    TPropagation t_propa = FORWARD | BACKWARD);
      int picard_iterations() const;

    protected:

      void contract_kth_slices(const tubex::Fnc& f,
                               TubeVector& tube,
                               int k,
                               TPropagation t_propa);
      void guess_kth_slices_envelope(const Fnc& f,
                                     TubeVector& tube,
                                     int k,
                                     TPropagation t_propa);

      float m_delta;
      int m_picard_iterations = 0;
  };
}

#endif
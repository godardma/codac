/* ============================================================================
 *  tubex-lib - Tube class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#include "tubex_Tube.h"
#include "tubex_TubeSlice.h"
#include "tubex_DomainException.h"
#include "tubex_StructureException.h"
#include "tubex_EmptyException.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  // Public methods

    // Definition

    TubeTree::TubeTree(const Interval& domain, const Interval& codomain) : TubeNode(domain, codomain)
    {
      m_first_tubenode = new TubeSlice(domain, codomain);
      m_second_tubenode = NULL;
    }

    TubeTree::TubeTree(const TubeTree& x) : TubeNode(x)
    {
      *this = x;
    }

    TubeTree::TubeTree(const TubeTree& x, const Interval& codomain) : TubeNode(x)
    {
      *this = x;
      set(codomain);
    }

    TubeTree::~TubeTree()
    {
      // Deleting gate between the two nodes
      TubeSlice *last_slice_first_node = getSlice(m_first_tubenode->nbSlices() - 1);
      if(((TubeSlice*)last_slice_first_node)->m_output_gate != NULL)
        delete ((TubeSlice*)last_slice_first_node)->m_output_gate;

      // Deleting nodes
      if(m_first_tubenode != NULL)
      {
        delete m_first_tubenode;
        m_first_tubenode = NULL;
      }

      if(m_second_tubenode != NULL)
      {
        delete m_second_tubenode;
        m_second_tubenode = NULL;
      }
    }

    TubeTree& TubeTree::operator=(const TubeTree& x)
    {
      TubeNode::operator=(x);

      m_tree_update_needed = x.m_tree_update_needed;

      if(m_first_tubenode != NULL)
      {
        delete m_first_tubenode;
        m_first_tubenode = NULL;
      }

      if(m_second_tubenode != NULL)
      {
        delete m_second_tubenode;
        m_second_tubenode = NULL;
      }

      if(x.getFirstTubeNode() != NULL)
      {
        if(x.getFirstTubeNode()->isSlice())
          m_first_tubenode = new TubeSlice(*((TubeSlice*)x.getFirstTubeNode()));

        else
          m_first_tubenode = new TubeTree(*((TubeTree*)x.getFirstTubeNode()));
      }

      if(x.getSecondTubeNode() != NULL)
      {
        if(x.getSecondTubeNode()->isSlice())
          m_second_tubenode = new TubeSlice(*((TubeSlice*)x.getSecondTubeNode()));

        else
          m_second_tubenode = new TubeTree(*((TubeTree*)x.getSecondTubeNode()));

        // Chaining slices (+ gate in between)

        TubeSlice::chainSlices(m_first_tubenode->getLastSlice(), m_second_tubenode->getFirstSlice());
        
        if(x.m_first_tubenode->getLastSlice()->m_output_gate != NULL)
        {
          double t = x.m_first_tubenode->domain().ub();
          m_first_tubenode->getLastSlice()->setOutputGate(*x.m_first_tubenode->getLastSlice()->m_output_gate);
        }
      }

      return *this;
    }

    // Slices structure
    
    bool TubeTree::isSlice() const
    {
      return false;
    }
    
    TubeNode* TubeTree::getFirstTubeNode() const
    {
      return m_first_tubenode;
    }
    
    TubeNode* TubeTree::getSecondTubeNode() const
    {
      return m_second_tubenode;
    }
    
    int TubeTree::sample(double t, const Interval& gate)
    {
      DomainException::check(*this, t);
      checkDataTree();
      int new_slices_nb = 1;

      if(m_second_tubenode == NULL)
      {
        if(m_first_tubenode->domain().lb() == t || m_first_tubenode->domain().ub() == t)
          return 0; // no degenerate slice

        TubeSlice slice(*(TubeSlice*)m_first_tubenode);
        TubeSlice *prev_slice = ((TubeSlice*)m_first_tubenode)->prevSlice();
        TubeSlice *next_slice = ((TubeSlice*)m_first_tubenode)->nextSlice();
        delete m_first_tubenode;

        m_first_tubenode = new TubeSlice(Interval(slice.domain().lb(), t), m_codomain);
        m_second_tubenode = new TubeSlice(Interval(t, slice.domain().ub()), m_codomain);

        TubeSlice::chainSlices(prev_slice, (TubeSlice*)m_first_tubenode);
        TubeSlice::chainSlices((TubeSlice*)m_first_tubenode, (TubeSlice*)m_second_tubenode);
        TubeSlice::chainSlices((TubeSlice*)m_second_tubenode, next_slice);

        if(!m_codomain.is_interior_subset(gate) && gate != Interval::ALL_REALS)
          ((TubeSlice*)m_first_tubenode)->setOutputGate(gate & m_codomain);
      }

      else
      {
        TubeNode **slice_ptr = NULL;

        if(m_first_tubenode->domain().contains(t))
        {
          if(m_first_tubenode->nbSlices() >= 2)
            new_slices_nb = ((TubeTree*)m_first_tubenode)->sample(t, gate);

          else
            slice_ptr = &m_first_tubenode;
        }

        else
        {
          if(m_second_tubenode->nbSlices() >= 2)
            new_slices_nb = ((TubeTree*)m_second_tubenode)->sample(t, gate);

          else if(m_second_tubenode != NULL)
            slice_ptr = &m_second_tubenode;
        }

        if(slice_ptr != NULL)
        {
          if((*slice_ptr)->domain().lb() == t || (*slice_ptr)->domain().ub() == t)
            return 0; // no degenerate slice
          
          TubeSlice slice(*(TubeSlice*)(*slice_ptr));
          TubeSlice *prev_slice = ((TubeSlice*)(*slice_ptr))->prevSlice();
          TubeSlice *next_slice = ((TubeSlice*)(*slice_ptr))->nextSlice();
          delete (*slice_ptr);

          *slice_ptr = new TubeTree(slice.domain(), slice.m_codomain);
          TubeSlice **first_slice = (TubeSlice**)&((TubeTree*)(*slice_ptr))->m_first_tubenode;
          TubeSlice **second_slice = (TubeSlice**)&((TubeTree*)(*slice_ptr))->m_second_tubenode;

          *first_slice = new TubeSlice(Interval(slice.domain().lb(), t), slice.m_codomain);
          *second_slice = new TubeSlice(Interval(t, slice.domain().ub()), slice.m_codomain);

          TubeSlice::chainSlices(prev_slice, *first_slice);
          TubeSlice::chainSlices(*first_slice, *second_slice);
          TubeSlice::chainSlices(*second_slice, next_slice);

          if(!(*slice_ptr)->m_codomain.is_interior_subset(gate) && gate != Interval::ALL_REALS)
            (*first_slice)->setOutputGate(gate & m_codomain);

          (*slice_ptr)->m_slices_number = 2;
        }
      }

      m_slices_number += new_slices_nb;
      return new_slices_nb;
    }
    
    void TubeTree::sample(const vector<double>& v_bounds)
    {
      if(v_bounds.empty())
        return;

      vector<double> v_first_bounds, v_last_bounds;

      int mid = v_bounds.size() / 2;
      for(int i = 0 ; i < v_bounds.size() ; i++)
      {
        if(i < mid) v_first_bounds.push_back(v_bounds[i]);
        else if(i <= mid) sample(v_bounds[i]);
        else v_last_bounds.push_back(v_bounds[i]);
      }

      sample(v_first_bounds);
      sample(v_last_bounds);
    }

    TubeSlice* TubeTree::getSlice(int slice_id)
    {
      return const_cast<TubeSlice*>(static_cast<const TubeTree*>(this)->getSlice(slice_id));
    }

    const TubeSlice* TubeTree::getSlice(int slice_id) const
    {
      DomainException::check(*this, slice_id);

      if(slice_id < m_first_tubenode->nbSlices())
        return m_first_tubenode->getSlice(slice_id);

      else
        return m_second_tubenode->getSlice(slice_id - m_first_tubenode->nbSlices());
    }

    TubeSlice* TubeTree::getSlice(double t)
    {
      return const_cast<TubeSlice*>(static_cast<const TubeTree*>(this)->getSlice(t));
    }
    
    const TubeSlice* TubeTree::getSlice(double t) const
    {
      DomainException::check(*this, t);
      return getSlice(input2index(t));
    }
    
    void TubeTree::getSlices(vector<const TubeSlice*>& v_slices) const
    {
      m_first_tubenode->getSlices(v_slices);
      m_second_tubenode->getSlices(v_slices);
    }
    
    int TubeTree::input2index(double t) const
    {
      DomainException::check(*this, t);

      if(t == m_domain.ub())
        return nbSlices() - 1;

      else if(t < m_first_tubenode->domain().ub())
        return m_first_tubenode->input2index(t);

      else
        return m_second_tubenode->input2index(t) + m_first_tubenode->nbSlices();
    }
    
    void TubeTree::getTubeNodes(vector<const TubeNode*> &v_nodes) const
    {
      v_nodes.push_back(static_cast<const TubeNode*>(this));
      m_first_tubenode->getTubeNodes(v_nodes);
      m_second_tubenode->getTubeNodes(v_nodes);
    }

    // Access values

    const Interval& TubeTree::codomain() const
    {
      checkDataTree();
      return m_codomain;
    }

    double TubeTree::volume() const
    {
      checkDataTree();
      return m_volume;
    }

    const Interval& TubeTree::operator[](int slice_id) const
    {
      // Write access is not allowed for this operator:
      // a further call to checkDataTree() is needed when values change,
      // this call cannot be garanteed with a direct access to m_codomain
      // For write access: use set()
      return getSlice(slice_id)->codomain();
    }

    const Interval TubeTree::operator[](double t) const
    {
      // Write access is not allowed for this operator:
      // a further call to checkDataTree() is needed when values change,
      // this call cannot be garanteed with a direct access to m_codomain
      // For write access: use set()
      return (*getSlice(t))[t];
    }

    const Interval TubeTree::operator[](const ibex::Interval& t) const
    {
      // Write access is not allowed for this operator:
      // a further call to checkDataTree() is needed when values change,
      // this call cannot be garanteed with a direct access to m_codomain
      // For write access: use set()

      Interval intersection = m_domain & t;

      if(intersection.is_empty())
        return Interval::EMPTY_SET;

      else if(t.is_degenerated())
        return (*this)[t.lb()];

      else if(intersection == m_domain)
        return codomain();

      else
      {
        if((m_first_tubenode->domain() & t).is_empty())
          return (*m_second_tubenode)[m_second_tubenode->domain() & t];

        else if((m_second_tubenode->domain() & t).is_empty())
          return (*m_first_tubenode)[m_first_tubenode->domain() & t];

        else
          return (*m_first_tubenode)[m_first_tubenode->domain() & t]
               | (*m_second_tubenode)[m_second_tubenode->domain() & t];
      }
    }

    const Interval TubeTree::interpol(double t, const TubeTree& derivative) const
    {
      return getSlice(t)->interpol(t, *derivative.getSlice(t));
    }

    const Interval TubeTree::interpol(const Interval& t, const TubeTree& derivative) const
    {
      DomainException::check(*this, t);
      DomainException::check(*this, derivative);
      EmptyException::check(derivative);

      // todo
    }
    
    Interval TubeTree::invert(const Interval& y, const Interval& search_domain) const
    {
      if(!domain().intersects(search_domain)) // to apply this function on a tube portion only
        return Interval::EMPTY_SET;

      else if(!codomain().intersects(y))
        return Interval::EMPTY_SET;

      else
      {
        if(isSlice())
          return search_domain & m_domain;

        else
          return m_first_tubenode->invert(y, search_domain) | m_second_tubenode->invert(y, search_domain);
      }
    }

    void TubeTree::invert(const Interval& y, vector<Interval> &v_t, const Interval& search_domain) const
    {
      return invert(y, v_t, search_domain, true);
    }

    double TubeTree::maxThickness()
    {
      int first_id_max_thickness;
      return maxThickness(first_id_max_thickness);
    }

    double TubeTree::maxThickness(int& first_id_max_thickness)
    {
      int i = 0;
      double max_thickness = 0.;

      TubeSlice *slice = getFirstSlice();
      while(slice != NULL)
      {
        if(slice->codomain().diam() > max_thickness)
        {
          max_thickness = slice->codomain().diam();
          first_id_max_thickness = i;
        }

        slice = slice->nextSlice();
        i++;
      }

      return max_thickness;
    }

    const pair<Interval,Interval> TubeTree::eval(const Interval& t) const
    {
      Interval intersection = m_domain & t;

      if(intersection.is_empty())
        return make_pair(Interval::EMPTY_SET, Interval::EMPTY_SET);

      else if(intersection.is_degenerated())
        return make_pair(Interval((*this)[intersection.lb()].lb()), Interval((*this)[intersection.lb()].ub()));

      else if(intersection == m_domain)
      {
        checkDataTree();
        return m_enclosed_bounds; // pre-computed
      }

      else
      {
        Interval inter_firstsubtube = m_first_tubenode->domain() & intersection;
        Interval inter_secondsubtube = m_second_tubenode->domain() & intersection;
        pair<Interval,Interval> ui_past = m_first_tubenode->eval(inter_firstsubtube);
        pair<Interval,Interval> ui_future = m_second_tubenode->eval(inter_secondsubtube);
        return make_pair(ui_past.first | ui_future.first, ui_past.second | ui_future.second);
      }
    }

    // Tests
    
    bool TubeTree::operator==(const TubeTree& x) const
    {
      return isEqual(x);
    }
    
    bool TubeTree::operator!=(const TubeTree& x) const
    {
      return isDifferent(x);
    }

    bool TubeTree::isSubset(const TubeTree& x) const
    {
      StructureException::check(*this, x);
      TubeSlice *slice = getFirstSlice();
      TubeSlice *outer_slice = getFirstSlice();

      while(slice != NULL)
      {
        if(!slice->isSubset(*outer_slice))
          return false;
        slice = slice->nextSlice();
        outer_slice = outer_slice->nextSlice();
      }

      return true;
    }

    bool TubeTree::isStrictSubset(const TubeTree& x) const
    {
      StructureException::check(*this, x);
      return isSubset(x) && (*this) != x;
    }
    
    bool TubeTree::isEmpty() const
    {
      TubeSlice *slice = getFirstSlice();

      while(slice != NULL)
      {
        if(slice->isEmpty())
          return true;
        slice = slice->nextSlice();
      }

      return false;
    }

    bool TubeTree::encloses(const Trajectory& x) const
    {
      TubeSlice *slice = getFirstSlice();

      while(slice != NULL)
      {
        if(!slice->encloses(x))
          return false;
        slice = slice->nextSlice();
      }

      return true;
    }

    // Setting values

    void TubeTree::set(const Interval& y)
    {
      TubeSlice *slice = getFirstSlice();

      int i = 0;
      while(slice != NULL)
      {
        slice->set(y);
        slice = slice->nextSlice();
        i++;
      }

      flagFutureTreeUpdate();
    }
    
    void TubeTree::set(const Interval& y, int slice_id)
    {
      getSlice(slice_id)->set(y);
      flagFutureTreeUpdate(slice_id);
    }
    
    void TubeTree::set(const Interval& y, double t)
    {
      sample(t);
      setGate(t, y);
    }
    
    void TubeTree::set(const Interval& y, const Interval& t)
    {
      if(t.is_degenerated())
        set(y, t.lb());

      else
      {
        sample(t.lb());
        sample(t.ub());

        int i = input2index(t.lb());
        TubeSlice *slice = getSlice(i);

        for( ; i <= input2index(t.ub()) && slice != NULL ; i++)
        {
          if((t & slice->domain()).is_degenerated())
            continue;
          
          slice->set(y);
          slice = slice->nextSlice();
          flagFutureTreeUpdate(i);
        }
      }
    }
    
    void TubeTree::setEmpty()
    {
      set(Interval::EMPTY_SET);
    }

    void TubeTree::setGate(double t, const Interval& gate)
    {
      TubeSlice *slice = getSlice(t);

      if(slice->domain().lb() == t)
        slice->setInputGate(gate);

      else if(slice->domain().ub() == t)
        slice->setOutputGate(gate);

      else
        throw Exception("TubeTree::setGate", "inexistent gate");
    }
    
    TubeTree& TubeTree::inflate(double rad)
    {
      TubeSlice *slice = getFirstSlice();
      while(slice != NULL)
      {
        slice->inflate(rad);
        slice = slice->nextSlice();
      }
      
      flagFutureTreeUpdate();
    }

    // Operators

    TubeTree& TubeTree::operator|=(const Trajectory& x)
    {
      DomainException::check(*this, x);

      TubeSlice *slice = getFirstSlice();
      while(slice != NULL)
      {
        *slice |= x;
        slice = slice->nextSlice();
      }

      flagFutureTreeUpdate();
    }

    TubeTree& TubeTree::operator|=(const TubeTree& x)
    {
      DomainException::check(*this, x);
      StructureException::check(*this, x);

      TubeSlice *slice = getFirstSlice();
      TubeSlice *x_slice = x.getFirstSlice();
      while(slice != NULL)
      {
        *slice |= *x_slice;
        slice = slice->nextSlice();
        x_slice = x_slice->nextSlice();
      }

      flagFutureTreeUpdate();
    }

    TubeTree& TubeTree::operator&=(const Trajectory& x)
    {
      DomainException::check(*this, x);

      TubeSlice *slice = getFirstSlice();
      while(slice != NULL)
      {
        *slice &= x;
        slice = slice->nextSlice();
      }

      flagFutureTreeUpdate();
    }

    TubeTree& TubeTree::operator&=(const TubeTree& x)
    {
      DomainException::check(*this, x);
      StructureException::check(*this, x);

      TubeSlice *slice = getFirstSlice();
      TubeSlice *x_slice = x.getFirstSlice();
      while(slice != NULL)
      {
        *slice &= *x_slice;
        slice = slice->nextSlice();
        x_slice = x_slice->nextSlice();
      }
      
      flagFutureTreeUpdate();
    }

    // Integration



  // Protected methods

    // Definition

    // Slices/tree structure

    void TubeTree::checkDataTree() const
    {
      if(!treeUpdateNeeded())
        return;

      if(!m_first_tubenode->isSlice())
        ((TubeTree*)m_first_tubenode)->checkDataTree();

      m_codomain = m_first_tubenode->codomain();
      m_volume = m_first_tubenode->volume();

      if(m_second_tubenode != NULL)
      {
        if(!m_second_tubenode->isSlice()) ((TubeTree*)m_second_tubenode)->checkDataTree();
        m_codomain |= m_second_tubenode->codomain();
        m_volume += m_second_tubenode->volume();
      }

      // Enclosed bounds
      m_enclosed_bounds = m_first_tubenode->eval();

      if(m_second_tubenode != NULL)
      {
        pair<Interval,Interval> ui_future = m_second_tubenode->eval();
        m_enclosed_bounds.first |= ui_future.first;
        m_enclosed_bounds.second |= ui_future.second;
      }

      m_tree_update_needed = false;
    }

    void TubeTree::flagFutureTreeUpdate(int slice_id) const
    {
      m_tree_update_needed = true;

      if(slice_id == -1)
      {
        if(!m_first_tubenode->isSlice()) ((TubeTree*)m_first_tubenode)->flagFutureTreeUpdate(-1);
        if(m_second_tubenode != NULL && !m_second_tubenode->isSlice()) ((TubeTree*)m_second_tubenode)->flagFutureTreeUpdate(-1);
      }

      else
      {
        DomainException::check(*this, slice_id);
        int mid_id = m_first_tubenode->nbSlices();

        if(slice_id < mid_id && !m_first_tubenode->isSlice())
          ((TubeTree*)m_first_tubenode)->flagFutureTreeUpdate(slice_id);

        else if(!m_second_tubenode->isSlice())
          ((TubeTree*)m_second_tubenode)->flagFutureTreeUpdate(slice_id - mid_id);
      }

      flagFuturePrimitiveUpdate(slice_id);
    }
    
    bool TubeTree::treeUpdateNeeded() const
    {
      return m_tree_update_needed;
    }
    
    // Access values

    void TubeTree::invert(const Interval& y, vector<ibex::Interval> &v_t, const Interval& search_domain, bool concatenate_results) const
    {
      v_t.clear();
      Interval intv_t_ctc = invert(y, search_domain);

      if(!intv_t_ctc.is_empty())
      {
        pair<Interval,Interval> enc_bounds = eval(intv_t_ctc);

        if(!concatenate_results)
        {
          if(enc_bounds.first.ub() > y.ub() || enc_bounds.second.lb() < y.lb())
          {
            // Bisection is needed
            vector<Interval> v1;
            m_first_tubenode->invert(y, v1, search_domain, false);
            v_t.insert(v_t.end(), v1.begin(), v1.end());
            vector<Interval> v2;
            m_second_tubenode->invert(y, v2, search_domain, false);
            v_t.insert(v_t.end(), v2.begin(), v2.end());
          }

          else
            v_t.push_back(intv_t_ctc);
        }

        else
        {
          vector<Interval> v;
          invert(y, v, search_domain, false);

          // Concatenation (solutions may be adjacent)
          int i = 0;
          while(i < v.size())
          {
            int j = i;
            Interval merge = v[i];

            while(j + 1 < v.size() && v[j].ub() == v[j + 1].lb())
            {
              j ++;
              merge |= v[j];
            }

            v_t.push_back(merge);
            i = j + 1;
          }
        }
      }
    }

    // Tests

    bool TubeTree::nodesAreEqual(const TubeNode* node1, const TubeNode* node2)
    {
      if(node1 != NULL || node2 != NULL)
      {
        if(node1 == NULL || node2 == NULL) // one is NULL, the other one is not
          return false;

        else // both are not NULL
        {
          if(node1->isSlice() && node2->isSlice()) // both are slices
          {
            if(!((TubeSlice*)node1)->TubeSlice::isEqual(*(TubeSlice*)node2))
              return false; // slices are not equal
          }

          else if(node1->isSlice() || node2->isSlice())
            return false; // one is a slice, the other one is not

          else if(!((TubeTree*)node1)->TubeTree::isEqual(*(TubeTree*)node2))
            return false; // both are trees, not equal
        }
      }

      return true;
    }

    bool TubeTree::isEqual(const TubeTree& x) const
    {
      StructureException::check(*this, x);

      return TubeNode::isEqual(x)
        && nodesAreEqual(m_first_tubenode, x.getFirstTubeNode())
        && nodesAreEqual(m_second_tubenode, x.getSecondTubeNode());
    }

    bool TubeTree::nodesAreDifferent(const TubeNode* node1, const TubeNode* node2)
    {
      if(node1 == NULL && node2 == NULL)
        return false;

      else if(node1 != NULL && node2 != NULL) // both are not NULL
      {
        if((node1->isSlice() && !node2->isSlice()) || (!node1->isSlice() && node2->isSlice()))
          return true;

        else // nodes are of same type
        {
          if(node1->isSlice()) // both are slices
            return ((TubeSlice*)node1)->TubeSlice::isDifferent(*(TubeSlice*)node2);

          else
            return ((TubeTree*)node1)->TubeTree::isDifferent(*(TubeTree*)node2);
        }
      }

      else // one is NULL, the other one is not
        return true;
    }
    
    bool TubeTree::isDifferent(const TubeTree& x) const
    {
      StructureException::check(*this, x);

      return TubeNode::isDifferent(x)
        || nodesAreDifferent(m_first_tubenode, x.getFirstTubeNode())
        || nodesAreDifferent(m_second_tubenode, x.getSecondTubeNode());
    }

    // Setting values

    // Operators

    // String

    // Contractors

    // Integration

    void TubeTree::checkPartialPrimitive() const
    {
      if(!m_primitive_update_needed)
        return;
      
      m_first_tubenode->checkPartialPrimitive();
      m_partial_primitive = m_first_tubenode->getPartialPrimitiveValue();

      if(m_second_tubenode != NULL)
      {
        m_second_tubenode->checkPartialPrimitive();
        m_partial_primitive.first |= m_second_tubenode->getPartialPrimitiveValue().first;
        m_partial_primitive.second |= m_second_tubenode->getPartialPrimitiveValue().second;
      }

      m_primitive_update_needed = false;
    }

    void TubeTree::flagFuturePrimitiveUpdate(int slice_id) const
    {
      m_primitive_update_needed = true;

      if(slice_id == -1)
      {
        m_first_tubenode->flagFuturePrimitiveUpdate(-1);
        if(m_second_tubenode != NULL) m_second_tubenode->flagFuturePrimitiveUpdate(-1);
      }

      else
      {
        DomainException::check(*this, slice_id);
        int mid_id = m_first_tubenode->nbSlices();

        m_first_tubenode->flagFuturePrimitiveUpdate(slice_id < mid_id ? slice_id : -1);

        if(m_second_tubenode != NULL && slice_id >= mid_id)
          m_second_tubenode->flagFuturePrimitiveUpdate(slice_id - mid_id);
      }
    }
}
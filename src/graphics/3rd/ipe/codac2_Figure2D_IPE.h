/** 
 *  \file codac2_Figure2D_IPE.h
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Simon Rohou, Maël Godard
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#pragma once

#include <vector>
#include <fstream>
#include <filesystem>
#include "codac2_Figure2D.h"
#include "codac2_OutputFigure2D.h"
#include "codac2_Vector.h"
#include "codac2_IntervalVector.h"
#include "vibes.h"

#include <ipebase.h>
#include <ipeimage.h>
#include <ipebitmap.h>
#include <ipedoc.h>
#include <ipestyle.h>
#include <ipeutils.h>
#include <ipeobject.h>
#include <ipepage.h>
#include <ipestyle.h>
#include <ipepswriter.h>
#include <ipeattributes.h>
#include <ipepath.h>
#include <ipeiml.h>

namespace codac2
{
  class Figure2D_IPE : public OutputFigure2D
  {
    public:

      Figure2D_IPE(const Figure2D& fig);
      ~Figure2D_IPE();
      void update_axes();
      void update_window_properties();
      void center_viewbox(const Vector& c, const Vector& r);

      // Geometric shapes
      void draw_point(const Vector& c, const StyleProperties& s = StyleProperties());
      void draw_box(const IntervalVector& x, const StyleProperties& s = StyleProperties());
      void draw_circle(const Vector& c, double r, const StyleProperties& s = StyleProperties());
      void draw_ring(const Vector& c, const Interval& r, const StyleProperties& s = StyleProperties());
      void draw_polyline(const std::vector<Vector>& x, float tip_length, const StyleProperties& s = StyleProperties());
      void draw_polygone(const std::vector<Vector>& x, const StyleProperties& s = StyleProperties());
      void draw_pie(const Vector& c, const Interval& r, const Interval& theta, const StyleProperties& s = StyleProperties());
      void draw_ellipse(const Vector& c, const Vector& ab, double theta, const StyleProperties& s = StyleProperties());

      // Robots
      void draw_tank(const Vector& x, float size, const StyleProperties& s = StyleProperties());
      void draw_AUV(const Vector& x, float size, const StyleProperties& s = StyleProperties());

    protected:

      ipe::AllAttributes setup_attributes(const StyleProperties& s);
      void add_color(const Color& c);

      double scale_x(double x) const;
      double scale_y(double y) const;
      double scale_length(double y) const;

      // std::ofstream _f, _f_temp_content;
      const double _ipe_grid_size = 500.;
      Vector _ratio { 1., 1. };

      std::string fig_name;
      std::shared_ptr<ipe::Document> doc;
      // ipe::Document * doc;
      ipe::Page * page = ipe::Page::basic();
      ipe::StyleSheet * stylesheet = new ipe::StyleSheet();
      ipe::Cascade * cascade = new ipe::Cascade();
  };
}
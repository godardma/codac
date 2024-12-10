/** 
 *  codac2_Figure2D_IPE.cpp
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Simon Rohou, Maël Godard
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#include <cstdio>
#include "codac2_Figure2D_IPE.h"
#include "codac2_math.h"

using namespace std;
using namespace codac2;


std::string ipe_str(const Color& c)
{
  return c.hex_str().substr(1);
}

int ipe_opacity(const Color& c)
{
  return (int)(10.*round(10.*(c.model()==Model::RGB ? (c[3]/255.):(c[3]/100.))));
}

ipe::AllAttributes Figure2D_IPE::setup_attributes(const StyleProperties& s)
{
  ipe::AllAttributes allAttributes;
  add_color(s.stroke_color);
  add_color(s.fill_color);
  allAttributes.iPathMode=ipe::EStrokedAndFilled;
  allAttributes.iStroke=ipe::Attribute(true,ipe::String(("codac_color_"+ipe_str(s.stroke_color)).c_str()));
  allAttributes.iFill=ipe::Attribute(true,ipe::String(("codac_color_"+ipe_str(s.fill_color)).c_str()));
  allAttributes.iOpacity=ipe::Attribute(true,ipe::String((std::to_string(ipe_opacity(s.fill_color))+"%").c_str()));
  allAttributes.iStrokeOpacity=ipe::Attribute(true,ipe::String((std::to_string(ipe_opacity(s.stroke_color))+"%").c_str()));
  allAttributes.iPen=ipe::Attribute(true,ipe::String("heavier"));
  return allAttributes;
}


void Figure2D_IPE::add_color(const Color& c)
{
  Color c_rgb = c.rgb();
  stylesheet->add(ipe::EColor,ipe::Attribute(true,ipe::String(("codac_color_"+c_rgb.hex_str().substr(1)).c_str())),ipe::Attribute(ipe::Color(c_rgb[0]*1000/255,c_rgb[1]*1000/255,c_rgb[2]*1000/255)));
}

Figure2D_IPE::Figure2D_IPE(const Figure2D& fig)
  : OutputFigure2D(fig),fig_name(fig.name())
{
  ipe::Platform::initLib(ipe::Platform::libVersion());
  doc = std::make_shared<ipe::Document>();

  doc->push_back(page);
  // insert basic stylesheet

  doc->replaceCascade(cascade);



  list<Color> codac_colors {
    Color::none(),
    Color::black(),
    Color::white(),
    Color::green(),
    Color::blue(),
    Color::cyan(),
    Color::yellow(),
    Color::red(),
    Color::dark_gray(),
    Color::purple(),
    Color::dark_green()
  };
  std::filesystem::path currentFilePath = __FILE__;
  std::FILE * file = std::fopen((currentFilePath.parent_path()/"codac_stylesheet.isy").string().c_str(), "r");
  ipe::FileSource source(file);
  ipe::StyleSheet * codac_stylesheet = ipe::ImlParser(source).parseStyleSheet();
  
  cascade->insert(0,codac_stylesheet);
  cascade->insert(1,stylesheet);
  
  stylesheet->setName("codac");

  //add colors
  for(const auto& ci : codac_colors)
    add_color(ci);

  // add opacities
  for(float i = 0. ; i < 1.01 ; i+=0.1)
    stylesheet->add(ipe::EOpacity,ipe::Attribute(true,ipe::String((std::to_string((int)(i*100.))+"%").c_str())),ipe::Attribute(ipe::Fixed::fromDouble(i)));
  
}

Figure2D_IPE::~Figure2D_IPE()
{
  std::FILE * file = std::fopen((fig_name+".xml").c_str(), "wb");
  ipe::FileStream stream (file);
  doc->saveAsXml(stream);
}

void Figure2D_IPE::update_axes()
{
  _ratio = {
    _ipe_grid_size/_fig.axes()[0].limits.diam(),
    _ipe_grid_size/_fig.axes()[1].limits.diam()
  };
}

void Figure2D_IPE::update_window_properties()
{

}

void Figure2D_IPE::center_viewbox([[maybe_unused]] const Vector& c, [[maybe_unused]] const Vector& r)
{
  assert(_fig.size() <= c.size() && _fig.size() <= r.size());
  assert(r.min_coeff() > 0.);
}

void Figure2D_IPE::draw_point(const Vector& c, const StyleProperties& s)
{
  assert(_fig.size() <= c.size());
  ipe::AllAttributes allAttributes = setup_attributes(s);
  ipe::Path path(allAttributes,ipe::Shape(ipe::Vector(scale_x(c[i()]),scale_y(c[j()])),scale_length(0.02)));
  page->append(ipe::TSelect::ENotSelected,0,path.clone());  
}

void Figure2D_IPE::draw_box(const IntervalVector& x, const StyleProperties& s)
{
  assert(_fig.size() <= x.size());
  ipe::AllAttributes allAttributes = setup_attributes(s);
  ipe::Path path(allAttributes,ipe::Shape(ipe::Rect(ipe::Vector(scale_x(x[i()].lb()),scale_y(x[j()].lb())),ipe::Vector(scale_x(x[i()].ub()),scale_y(x[j()].ub())))));
  page->append(ipe::TSelect::ENotSelected,0,path.clone());
}

void Figure2D_IPE::draw_circle(const Vector& c, double r, const StyleProperties& s)
{
  assert(_fig.size() <= c.size());
  assert(r > 0.);
  ipe::AllAttributes allAttributes = setup_attributes(s);
  ipe::Path path(allAttributes,ipe::Shape(ipe::Vector(scale_x(c[i()]),scale_y(c[j()])),scale_length(r)));
  page->append(ipe::TSelect::ENotSelected,0,path.clone());
}

void Figure2D_IPE::draw_ring(const Vector& c, const Interval& r, const StyleProperties& s)
{
  assert(_fig.size() <= c.size());
  assert(!r.is_empty() && r.lb() >= 0.);

  ipe::AllAttributes allAttributes = setup_attributes(s);

  ipe::Shape shap0 (ipe::Vector(scale_x(c[i()]),scale_y(c[j()])),scale_length(r.ub()));
  ipe::Ellipse* ellipse = new ipe::Ellipse(ipe::Matrix(scale_length(r.lb()),0.,0.,scale_length(r.lb()),scale_x(c[i()]),scale_y(c[j()])));
  shap0.appendSubPath(ellipse);
  ipe::Path path(allAttributes,shap0);
  page->append(ipe::TSelect::ENotSelected,0,path.clone());
}

void Figure2D_IPE::draw_polyline(const std::vector<Vector>& x, float tip_length, const StyleProperties& s)
{
  assert(x.size() > 1);
  assert(tip_length >= 0.);


  ipe::AllAttributes allAttributes = setup_attributes(s);
  if (tip_length > 2e-3*_fig.scaled_unit())
  {
    allAttributes.iFArrow = true;
  }
  ipe::Curve *curve = new ipe::Curve();
  for(size_t k = 0 ; k < x.size()-1 ; k++)
  {
    assert(_fig.size() <= x[k].size());
    curve->appendSegment(ipe::Vector(scale_x(x[k][i()]),scale_y(x[k][j()])),ipe::Vector(scale_x(x[k+1][i()]),scale_y(x[k+1][j()])));
  }

  ipe::Shape shape;
  shape.appendSubPath(curve);
  page->append(ipe::TSelect::ENotSelected,0,ipe::Path(allAttributes,shape,true).clone());
}

void Figure2D_IPE::draw_polygone(const std::vector<Vector>& x, const StyleProperties& s)
{
  draw_polyline(x,0.,s);
}

void Figure2D_IPE::draw_pie(const Vector& c, const Interval& r, const Interval& theta, const StyleProperties& s)
{
  assert(_fig.size() <= c.size());
  assert(r.lb() >= 0.);

  ipe::AllAttributes allAttributes = setup_attributes(s);

  Vector point1 ({r.lb() * std::cos(theta.lb()), r.lb() * std::sin(theta.lb())});
  Vector point2 ({r.ub() * std::cos(theta.lb()), r.ub() * std::sin(theta.lb())});
  Vector point3 ({r.ub() * std::cos(theta.ub()), r.ub() * std::sin(theta.ub())});
  Vector point4 ({r.lb() * std::cos(theta.ub()), r.lb() * std::sin(theta.ub())});
  ipe::Curve *curve = new ipe::Curve();
  curve->appendSegment(ipe::Vector(scale_x(c[i()]+point1[i()]),scale_y(c[j()]+point1[j()])),ipe::Vector(scale_x(c[i()]+point2[i()]),scale_y(c[j()]+point2[j()])));
  curve->appendArc(ipe::Matrix(scale_length(r.ub()),0.,0.,scale_length(r.ub()),scale_x(c[i()]),scale_y(c[j()])),ipe::Vector(scale_x(c[i()]+point2[i()]),scale_y(c[j()]+point2[j()])),ipe::Vector(scale_x(c[i()]+point3[i()]),scale_y(c[j()]+point3[j()])));
  curve->appendSegment(ipe::Vector(scale_x(c[i()]+point3[i()]),scale_y(c[j()]+point3[j()])),ipe::Vector(scale_x(c[i()]+point4[i()]),scale_y(c[j()]+point4[j()])));
  curve->appendArc(ipe::Matrix(scale_length(r.lb()),0.,0.,-scale_length(r.lb()),scale_x(c[i()]),scale_y(c[j()])),ipe::Vector(scale_x(c[i()]+point4[i()]),scale_y(c[j()]+point4[j()])),ipe::Vector(scale_x(c[i()]+point1[i()]),scale_y(c[j()]+point1[j()])));
  ipe::Shape shape;
  shape.appendSubPath(curve);
  page->append(ipe::TSelect::ENotSelected,0,ipe::Path(allAttributes,shape).clone());
}

void Figure2D_IPE::draw_ellipse(const Vector& c, const Vector& ab, double theta, const StyleProperties& s)
{
  assert(c.size() == 2);
  assert(ab.size() == 2);

  ipe::AllAttributes allAttributes = setup_attributes(s);

  ipe::Ellipse * ellipse = new ipe::Ellipse(ipe::Matrix(scale_length(ab[0])*std::cos(theta),scale_length(ab[0]) * std::sin(theta),- scale_length(ab[1]) * std::sin(theta),scale_length(ab[1]) * std::cos(theta),scale_x(c[i()]),scale_y(c[j()])));
  ipe::Shape shape;
  shape.appendSubPath(ellipse);
  page->append(ipe::TSelect::ENotSelected,0,ipe::Path(allAttributes,shape).clone());
}

void Figure2D_IPE::draw_tank(const Vector& x, float size, const StyleProperties& s)
{
  assert(_fig.size() <= x.size()+1);
  assert(j()+1 < x.size());
  assert(size >= 0.);
  
  float length=size/4.0; // from VIBes : initial vehicle's length is 4

  ipe::AllAttributes allAttributes = setup_attributes(s);

  ipe::Curve *curve = new ipe::Curve();
  curve->appendSegment(ipe::Vector(1,-1.5),ipe::Vector(-1,-1.5));
  curve->appendSegment(ipe::Vector(-1,-1.5),ipe::Vector(0,-1.5));
  curve->appendSegment(ipe::Vector(0,-1.5),ipe::Vector(0,-1));
  curve->appendSegment(ipe::Vector(0,-1),ipe::Vector(-1,-1));
  curve->appendSegment(ipe::Vector(-1,-1),ipe::Vector(-1,1));
  curve->appendSegment(ipe::Vector(-1,1),ipe::Vector(0,1));
  curve->appendSegment(ipe::Vector(0,1),ipe::Vector(0,1.5));
  curve->appendSegment(ipe::Vector(0,1.5),ipe::Vector(-1,1.5));
  curve->appendSegment(ipe::Vector(-1,1.5),ipe::Vector(1,1.5));
  curve->appendSegment(ipe::Vector(1,1.5),ipe::Vector(0,1.5));
  curve->appendSegment(ipe::Vector(0,1.5),ipe::Vector(0,1));
  curve->appendSegment(ipe::Vector(0,1),ipe::Vector(3,0.5));
  curve->appendSegment(ipe::Vector(3,0.5),ipe::Vector(3,-0.5));
  curve->appendSegment(ipe::Vector(3,-0.5),ipe::Vector(0,-1));
  curve->appendSegment(ipe::Vector(0,-1),ipe::Vector(0,-1.5));

  ipe::Shape shape;
  shape.appendSubPath(curve);
  ipe::Path path(allAttributes,shape);
  ipe::Matrix matrix(scale_length(length)*std::cos(x[j()+1]),scale_length(length)*std::sin(x[j()+1]),-scale_length(length)*std::sin(x[j()+1]),scale_length(length)*std::cos(x[j()+1]),scale_x(x[i()]),scale_y(x[j()]));
  path.setMatrix(matrix);
  page->append(ipe::TSelect::ENotSelected,0,path.clone());
}

void Figure2D_IPE::draw_AUV(const Vector& x, float size, const StyleProperties& s)
{
  assert(_fig.size() <= x.size()+1);
  assert(j()+1 < x.size());
  assert(size >= 0.);

  float length=size/7.0; // from VIBes : initial vehicle's length is 7

  ipe::Group *group = new ipe::Group();
  ipe::Matrix matrix(scale_length(length)*std::cos(x[j()+1]),scale_length(length)*std::sin(x[j()+1]),-scale_length(length)*std::sin(x[j()+1]),scale_length(length)*std::cos(x[j()+1]),scale_x(x[i()]),scale_y(x[j()]));
  ipe::AllAttributes allAttributes = setup_attributes(s);

  // Body

  ipe::Curve *curve_body = new ipe::Curve();
  curve_body->appendSegment(ipe::Vector(-4,0),ipe::Vector(-2,1));
  curve_body->appendSegment(ipe::Vector(-2,1),ipe::Vector(2,1));
  curve_body->appendSegment(ipe::Vector(2,1),ipe::Vector(2.17365,0.984808));
  curve_body->appendSegment(ipe::Vector(2.17365,0.984808),ipe::Vector(2.34202,0.939693));
  curve_body->appendSegment(ipe::Vector(2.34202,0.939693),ipe::Vector(2.5,0.866025));
  curve_body->appendSegment(ipe::Vector(2.5,0.866025),ipe::Vector(2.64279,0.766044));
  curve_body->appendSegment(ipe::Vector(2.64279,0.766044),ipe::Vector(2.76604,0.642788));
  curve_body->appendSegment(ipe::Vector(2.76604,0.642788),ipe::Vector(2.86603,0.5));
  curve_body->appendSegment(ipe::Vector(2.86603,0.5),ipe::Vector(2.93969,0.34202));
  curve_body->appendSegment(ipe::Vector(2.93969,0.34202),ipe::Vector(2.98481,0.173648));
  curve_body->appendSegment(ipe::Vector(2.98481,0.173648),ipe::Vector(3,0));
  curve_body->appendSegment(ipe::Vector(3,0),ipe::Vector(2.98481,-0.173648));
  curve_body->appendSegment(ipe::Vector(2.98481,-0.173648),ipe::Vector(2.93969,-0.34202));
  curve_body->appendSegment(ipe::Vector(2.93969,-0.34202),ipe::Vector(2.86603,-0.5));
  curve_body->appendSegment(ipe::Vector(2.86603,-0.5),ipe::Vector(2.76604,-0.642788));
  curve_body->appendSegment(ipe::Vector(2.76604,-0.642788),ipe::Vector(2.64279,-0.766044));
  curve_body->appendSegment(ipe::Vector(2.64279,-0.766044),ipe::Vector(2.5,-0.866025));
  curve_body->appendSegment(ipe::Vector(2.5,-0.866025),ipe::Vector(2.34202,-0.939693));
  curve_body->appendSegment(ipe::Vector(2.34202,-0.939693),ipe::Vector(2.17365,-0.984808));
  curve_body->appendSegment(ipe::Vector(2.17365,-0.984808),ipe::Vector(2,-1));
  curve_body->appendSegment(ipe::Vector(2,-1),ipe::Vector(-2,-1));
  curve_body->appendSegment(ipe::Vector(-2,-1),ipe::Vector(-4,0));

  ipe::Shape shape_body;
  shape_body.appendSubPath(curve_body);
  ipe::Path path_body(allAttributes,shape_body);
  path_body.setMatrix(matrix);
  group->push_back(path_body.clone());

  // Propulsion unit

  ipe::Curve *curve_propeller = new ipe::Curve();
  curve_propeller->appendSegment(ipe::Vector(-4,1),ipe::Vector(-3.25,1));
  curve_propeller->appendSegment(ipe::Vector(-3.25,1),ipe::Vector(-3.25,-1));
  curve_propeller->appendSegment(ipe::Vector(-3.25,-1),ipe::Vector(-4,-1));
  curve_propeller->appendSegment(ipe::Vector(-4,-1),ipe::Vector(-4,1));

  ipe::Shape shape_propeller;
  shape_propeller.appendSubPath(curve_propeller);
  ipe::Path path_propeller(allAttributes,shape_propeller);
  path_propeller.setMatrix(matrix);
  group->push_back(path_propeller.clone());

  page->append(ipe::TSelect::ENotSelected,0,group->clone());
}

double Figure2D_IPE::scale_x(double x) const
{
  return (x-_fig.axes()[0].limits.lb())*_ratio[0];
}

double Figure2D_IPE::scale_y(double y) const
{
  return (y-_fig.axes()[1].limits.lb())*_ratio[1];
}

double Figure2D_IPE::scale_length(double x) const
{
  // Assuming _ratio[0] == _ratio[1]
  return x*_ratio[0];
}
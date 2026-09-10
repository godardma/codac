#include <codac>

using namespace std;
using namespace codac2;

int main()
{
  // {psi0,Sigma} is a gnomonic atlas of the box [-1,1]^2
  VectorVar X(1);
  AnalyticFunction psi0 ({X},{X[0],1});

  OctaSym id ({1, 2});
  OctaSym s ({-2, 1});

  vector<OctaSym> Sigma ({id,s,s*s,s.invert()});

  // In this box, we consider that the x-axis is the width of the linear sensor
  // and the y-axis is the time
  VectorVar y (2);
  auto t = 4.0*y[0];
  auto L = 0.5*y[1];

  // The image of [-1,1]^2 by both functions is the area swept 
  AnalyticFunction f1 ({y},{t+L*(-cos(t)),sin(t)+L});
  AnalyticFunction f2 ({y},{sin(t)+L,2.+t+L*(-cos(t))});

  // For the SepImage, we need a contractor on the initial set
  IntervalVector X0 = IntervalVector::constant(2,{-1,1});
  CtcWrapper ctc_in (X0);
  
  // Separator on the area seen by a first robot
  SepImage sep1 (f1,psi0,Sigma,0.1,ctc_in);
  // Separator on the area seen by a second robot
  SepImage sep2 (f2,psi0,Sigma,0.1,ctc_in);

  // Graphical setup

  IntervalVector Y0 ({{-6,5},{-4,7}});

  Figure2D fig1 ("robot1",GraphicOutput::VIBES);
  Figure2D fig2 ("robot2",GraphicOutput::VIBES);
  Figure2D fig_union ("union, seen by at least one",GraphicOutput::VIBES);
  Figure2D fig_inter ("intersection, seen by both",GraphicOutput::VIBES);

  fig1.set_axes(Y0);
  fig2.set_axes(Y0);
  fig_union.set_axes(Y0);
  fig_inter.set_axes(Y0);

  fig1.set_window_properties({50,50},{500,500});
  fig2.set_window_properties({600,50},{500,500});
  fig_union.set_window_properties({50,600},{500,500});
  fig_inter.set_window_properties({600,600},{500,500});

  fig1.pave(Y0,sep1,0.1);
  fig2.pave(Y0,sep2,0.1);
  fig_union.pave(Y0,sep1|sep2,0.1);
  fig_inter.pave(Y0,sep1&sep2,0.1);
}
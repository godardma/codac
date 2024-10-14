#include <codac>

using namespace std;
using namespace codac2;

int main(){
    Figure2D fig("fig",GraphicOutput::VIBES);
    fig.set_window_properties({1000,100},{800,800});
    fig.set_axes(axis(0,{-0.5,5.5}), axis(1,{-0.5,5.5}));

    // VIBes syntax
    fig.draw_box(IntervalVector{{0,0.9},{0,0.9}},(string)"blue");
    fig.draw_box(IntervalVector{{1,1.9},{0,0.9}},(string)"red[blue]");

    // Predefined styles
    fig.draw_box(IntervalVector{{0,0.9},{1,1.9}},StyleProperties::inside());
    fig.draw_box(IntervalVector{{1,1.9},{1,1.9}},StyleProperties::outside());
    fig.draw_box(IntervalVector{{2,2.9},{1,1.9}},StyleProperties::boundary());

    // Custom styles
    
    // Edge only
    fig.draw_box(IntervalVector{{0,0.9},{2,2.9}},Color::blue());
    fig.draw_box(IntervalVector{{1,1.9},{2,2.9}},Color("#ff0000"));
    fig.draw_box(IntervalVector{{2,2.9},{2,2.9}},{Color::green(0.5)});
    fig.draw_box(IntervalVector{{3,3.9},{2,2.9}},{Color("#00ffff55")});
    fig.draw_box(IntervalVector{{4,4.9},{2,2.9}},{"#ffff00dd"});

    // Edge and fill
    fig.draw_box(IntervalVector{{0,0.9},{3,3.9}},{Color::green(),Color("#00ffff55")});
    fig.draw_box(IntervalVector{{1,1.9},{3,3.9}},{"#ffff00","#ff0000dd"});
}
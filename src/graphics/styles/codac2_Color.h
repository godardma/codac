/** 
 *  \file codac2_Color.h
 * ----------------------------------------------------------------------------
 *  \date       2024
 *  \author     Simon Rohou
 *  \copyright  Copyright 2024 Codac Team
 *  \license    GNU Lesser General Public License (LGPL)
 */

#pragma once

#include <string>
#include <iomanip>
#include <map>
#include"codac2_assert.h"

namespace codac2
{
  /**
   * \struct Color
   * \brief Represents an RGB value
   */

  
  struct Color
  {
    float r;              ///< red, value between 0. and 1.
    float g;              ///< green, value between 0. and 1.
    float b;              ///< blue, value between 0. and 1.
    float alpha = 1.;     ///< opacity, value between 0. (transparent) and 1. (opaque)
    std::string hex_str;  ///< represents an RGB value in a HTML standard



    explicit Color(float r_, float g_, float b_, float alpha_ = 1.);
    explicit Color(int r_, int g_, int b_, int alpha_ = 255);
    explicit Color(const std::string& hex_str_);

    static Color none()                      { return Color(255, 255, 255, 0               ); };
    static Color black(float alpha = 1)      { return Color(0,   0,   0,   (int)(alpha*255)); };
    static Color white(float alpha = 1)      { return Color(255, 255, 255, (int)(alpha*255)); };
    static Color green(float alpha = 1)      { return Color(144, 242, 0,   (int)(alpha*255)); };
    static Color blue(float alpha = 1)       { return Color(0,   98,  198, (int)(alpha*255)); };
    static Color cyan(float alpha = 1)       { return Color(75,  207, 250, (int)(alpha*255)); };
    static Color yellow(float alpha = 1)     { return Color(255, 211, 42,  (int)(alpha*255)); };
    static Color red(float alpha = 1)        { return Color(209, 59,  0,   (int)(alpha*255)); };
    static Color dark_gray(float alpha = 1)  { return Color(112, 112, 112, (int)(alpha*255)); };
    static Color purple(float alpha = 1)     { return Color(154, 0,   170, (int)(alpha*255)); };
    static Color dark_green(float alpha = 1) { return Color(94,  158, 0,   (int)(alpha*255)); };

    static std::map<std::string, Color> vibes_colors(){
      return {
        {"white",Color("#ffffff")},
        {"black", Color("#000000")},
        {"red", Color("#ff0000")},
        {"darkRed", Color("#800000")},
        {"blue", Color("#0000ff")},
        {"darkGreen", Color("#008000")},
        {"green", Color("#00ff00")},
        {"darkBlue", Color("#0000ff")},
        {"cyan", Color("#00ffff")},
        {"darkCyan", Color("#008080")},
        {"magenta", Color("#ff00ff")},
        {"darkMagenta", Color("#800080")},
        {"yellow", Color("#ffff00")},
        {"darkYellow", Color("#808000")},
        {"gray", Color("#a0a0a4")},
        {"darkGray", Color("#808080")},
        {"lightGray", Color("#c0c0c0")},
        {"transparent", Color("#00000000")}
      };
    };
  };
}
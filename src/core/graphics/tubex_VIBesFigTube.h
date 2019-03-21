/** 
 *  \file
 *  VIBesFigTube class
 * ----------------------------------------------------------------------------
 *  \date       2016
 *  \author     Simon Rohou
 *  \copyright  Copyright 2019 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#ifndef __TUBEX_VIBESFIGTUBE_H__
#define __TUBEX_VIBESFIGTUBE_H__

#include "tubex_VIBesFig.h"
#include "tubex_Tube.h"
#include "tubex_Slice.h"
#include "tubex_Trajectory.h"

namespace tubex
{
  #define DEFAULT_TUBE_NAME         "[?](·)"
  #define DEFAULT_TRAJ_NAME         "?(·)"
  #define TRAJ_NB_DISPLAYED_POINTS  10000
  
  // HTML color codes:
  #define DEFAULT_TRAJ_COLOR        "#004257"
  #define DEFAULT_FRGRND_COLOR      "#a2a2a2[#a2a2a2]"
  #define DEFAULT_BCKGRND_COLOR     "#d2d2d2[#d2d2d2]"
  #define DEFAULT_SLICES_COLOR      "#828282[#F0F0F0]"
  #define DEFAULT_GATES_COLOR       "#0084AF[#0084AF]"
  #define DEFAULT_POLYGONS_COLOR    "#00536E[#2696BA]"

  /**
   * \enum TubeColorType
   * \brief Defines a set of colors for tube display
   */
  enum TubeColorType { FOREGROUND, BACKGROUND, SLICES, GATES, POLYGONS };

  /**
   * \class VIBesFigTube
   * \brief Two-dimensional graphical item to display scalar tubes or trajectories
   *
   * One figure is linked to some tube or trajectory pointers, so that
   * any update on these objects can be easily displayed on the figure. 
   */
  class VIBesFigTube : public VIBesFig
  {
    public:

      /// \name Basics
      /// @{

      /**
       * \brief Creates a VIBesFigTube
       *
       * \param fig_name name of the figure as displayed in the window title
       * \param tube an optional const pointer to the tube to be displayed (`NULL` by default)
       * \param traj an optional const pointer to a trajectory to be displayed (`NULL` by default)
       */
      VIBesFigTube(const std::string& fig_name, const Tube *tube = NULL, const Trajectory *traj = NULL);

      /**
       * \brief VIBesFigTube destructor
       */
      ~VIBesFigTube();

      /**
       * \brief Displays this figure
       */
      void show();

      /**
       * \brief Displays this figure with optional details
       *
       * \param detail_slices if `true`, each slice will be displayed as a box,
       *        otherwise, only polygon envelopes of the tubes will be shown (fast display)
       */
      void show(bool detail_slices);

      /// @}
      /// \name Handling tubes
      /// @{

      /**
       * \brief Adds a tube object to this figure
       *
       * \param tube a const pointer to a Tube object to be displayed
       * \param name a name to identify this object
       * \param color_frgrnd an optional color for the current values of the tube
       * \param color_bckgrnd an optional color for the previous values of the tube, before any new contraction
       */
      void add_tube(const Tube *tube, const std::string& name, const std::string& color_frgrnd = DEFAULT_FRGRND_COLOR, const std::string& color_bckgrnd = DEFAULT_BCKGRND_COLOR);

      /**
       * \brief Sets a new name for a tube
       *
       * \param tube the const pointer to the Tube object to be renamed
       * \param name a new name to identify this object
       */
      void set_tube_name(const Tube *tube, const std::string& name);

      /**
       * \brief Links a tube to its derivative for display purposes
       *
       * When available, a derivative tube \f$[\dot{x}](\cdot)\f$ allows to draw slices of
       * \f$[x](\cdot)\f$ as polygons, thus displaying a thinner envelope of the tube \f$[x](\cdot)\f$. 
       *
       * \param tube the const pointer to the Tube object that will be displayed
       * \param derivative the const pointer its derivative set
       */
      void set_tube_derivative(const Tube *tube, const Tube *derivative);

      /**
       * \brief Sets color properties for a given tube
       *
       * \param tube the const pointer to the Tube object for which the colors will be set
       * \param color_frgrnd a color for the current values of the tube
       * \param color_bckgrnd a color for the previous values of the tube, before any new contraction (gray by default)
       */
      void set_tube_color(const Tube *tube, const std::string& color_frgrnd, const std::string& color_bckgrnd = DEFAULT_BCKGRND_COLOR);

      /**
       * \brief Sets color properties for a given tube
       *
       * This method allows to change the display of slices, borders, gates, etc.
       *
       * \param tube the const pointer to the Tube object for which the colors will be set
       * \param color_type the `TubeColorType` key for which the value will be set
       * \param color the new color to be specified for the given type
       */
      void set_tube_color(const Tube *tube, int color_type, const std::string& color);

      /**
       * \brief Removes a tube from this figure
       *
       * \todo automatically clear the figure (for now, the tube is only removed
       *       from the list of objects to be shown afterwards)
       *
       * \note the object will not be deleted
       *
       * \param tube the const pointer to the Tube object to be removed
       */
      void remove_tube(const Tube *tube);

      /// @}
      /// \name Handling trajectories
      /// @{

      /**
       * \brief Adds a trajectory object to this figure
       *
       * \param traj a const pointer to a Trajectory object to be displayed
       * \param name a name to identify this object
       * \param color an optional color to draw this trajectory
       */
      void add_trajectory(const Trajectory *traj, const std::string& name, const std::string& color = DEFAULT_TRAJ_COLOR);

      /**
       * \brief Sets a new name for a trajectory
       *
       * \param traj the const pointer to the Trajectory object to be renamed
       * \param name a new name to identify this object
       */
      void set_trajectory_name(const Trajectory *traj, const std::string& name);

      /**
       * \brief Sets color properties for a given trajectory
       *
       * \param traj the const pointer to the Trajectory object for which the color will be set
       * \param color a color to draw this trajectory
       */
      void set_trajectory_color(const Trajectory *traj, const std::string& color);

      /**
       * \brief Removes a trajectory from this figure
       *
       * \todo automatically clear the figure (for now, the trajectory is only removed
       *       from the list of objects to be shown afterwards)
       *
       * \note the object will not be deleted
       *
       * \param traj the const pointer to the Trajectory object to be removed
       */
      void remove_trajectory(const Trajectory *traj);

      /// @}

    protected:

      /**
       * \brief Creates the VIBes group of `color_type` related to the given tube
       *
       * \param tube the const pointer to the Tube object for which a group will be created
       * \param color_type the `TubeColorType` key related to the new group
       */
      void create_group_color(const Tube *tube, int color_type);

      /**
       * \brief Creates all the VIBes groups related to the given tube
       *
       * \param tube the const pointer to the Tube object for which the groups will be created
       */
      void create_groups_color(const Tube *tube);

      /**
       * \brief Draws a tube
       *
       * \param tube a const pointer to a Tube object to be shown
       * \param detail_slices if `true`, each slice will be displayed as a box,
       *        otherwise, only polygon envelopes of the tubes will be shown (fast display)
       * \return the box hull of the displayed object
       */
      const ibex::IntervalVector draw_tube(const Tube *tube, bool detail_slices = false);

      /**
       * \brief Draws a slice
       *
       * \param slice the slice to be displayed
       * \param params VIBes parameters related to the slice (for groups)
       */
      void draw_slice(const Slice& slice, const vibes::Params& params);

      /**
       * \brief Draws a slice knowing its derivative
       *
       * \param slice the slice to be displayed
       * \param deriv_slice its derivative set
       * \param params_slice VIBes parameters related to the slice (for groups)
       * \param params_polygon VIBes parameters related to the polygon computed from the derivative
       */
      void draw_slice(const Slice& slice, const Slice& deriv_slice, const vibes::Params& params_slice, const vibes::Params& params_polygon);

      /**
       * \brief Draws a gate
       *
       * \param gate the codomain
       * \param t the domain input
       * \param params VIBes parameters related to the gate (for groups)
       */
      void draw_gate(const ibex::Interval& gate, double t, const vibes::Params& params);

      /**
       * \brief Draws a trajectory
       *
       * \param traj the const pointer to the Trajectory object to be shown
       * \param points_size optional display mode, for which the trajectory is not a line but a set of points of a given size
       * \return the box hull of the displayed object
       */
      const ibex::IntervalVector draw_trajectory(const Trajectory *traj, float points_size = 0.);

    protected:

      /**
       * \struct FigTubeParams
       * \brief Specifies some parameters related to a Tube display
       */
      struct FigTubeParams
      {
        std::string name; //!< string identifier of the tube
        std::map<int,std::string> m_colors; //!< map of colors `<TubeColorType,html_color_code>`
        const Tube *tube_copy = NULL; //!< to display previous values in background, before any new contraction
        const Tube *tube_derivative = NULL; //!< to display thinner envelopes (polygons) enclosed by the slices
      };

      /**
       * \struct FigTrajParams
       * \brief Specifies some parameters related to a Trajectory display
       */
      struct FigTrajParams
      {
        std::string name; //!< string identifier of the trajectory
        std::string color; //!< color of the trajectory
      };

      std::map<const Tube*,FigTubeParams> m_map_tubes; //!< map of Tube objects to be displayed, together with parameters
      std::map<const Trajectory*,FigTrajParams> m_map_trajs; //!< map of Trajectory objects to be displayed, together with parameters

      friend class VIBesFigTubeVector;
  };
}

#endif
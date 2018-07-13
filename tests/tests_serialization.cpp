#include "tests.h"
#include <cstdio>

using namespace Catch;
using namespace Detail;
using namespace std;
using namespace ibex;
using namespace tubex;

TEST_CASE("serialization/deserialization of Tube")
{
  SECTION("No gates")
  {
    Tube tube1 = tube_test_1();
    string filename = "test_serialization_nogates.tube";
    tube1.serialize(filename);

    Trajectory traj;
    vector<Trajectory> v_trajs;
    CHECK_THROWS(Tube tube2(filename, traj););
    CHECK_THROWS(Tube tube3(filename, v_trajs););
    Tube tube4(filename);
    remove(filename.c_str());
    CHECK(tube1 == tube4);
  }

  SECTION("With gates")
  {
    Tube tube1 = tube_test_1();

    CHECK(tube1[0.] == Interval(4.,8.));
    CHECK(tube1[46.] == Interval(-1.,3.));
    CHECK(tube1[3.] == Interval(1.,4.));

    // todo: test a set value at 3.9

    tube1.set(Interval(2.,3.), 3.);
    tube1.set(Interval(7.), 0.);
    tube1.set(Interval::EMPTY_SET, 46.);

    CHECK(tube1[0.] == Interval(7.));
    CHECK(tube1[46.] == Interval::EMPTY_SET);
    CHECK(tube1[3.] == Interval(2.,3.));

    string filename = "test_serialization_gates.tube";
    tube1.serialize(filename);
    Tube tube2(filename);
    remove(filename.c_str());
    CHECK(tube1 == tube2);

    CHECK(tube2[0.] == Interval(7.));
    CHECK(tube2[46.] == Interval::EMPTY_SET);
    CHECK(tube2[3.] == Interval(2.,3.));
  }

  SECTION("With trajectories")
  {
    Tube tube1 = tube_test_1();
    Trajectory traj1, traj2, traj3;
    for(int i = 0 ; i < tube1.nb_slices() ; i++)
    {
      traj1.set(tube1.get_slice(i)->domain().mid(), tube1[i].mid());
      traj2.set(tube1.get_slice(i)->domain().mid(), tube1[i].lb());
      traj3.set(tube1.get_slice(i)->domain().mid(), tube1[i].ub());
    }

    string filename = "test_serialization_traj.tube";
    tube1.serialize(filename, traj1);
    Trajectory traj4;
    Tube tube2(filename, traj4);
    remove(filename.c_str());
    CHECK(tube1 == tube2);
    CHECK(traj1 == traj4);

    vector<Trajectory> v_trajs;
    v_trajs.push_back(traj1);
    v_trajs.push_back(traj2);
    v_trajs.push_back(traj3);

    filename = "test_serialization_trajs.tube";
    tube1.serialize(filename, v_trajs);
    vector<Trajectory> v_trajs_2;
    Tube tube3(filename, v_trajs_2);
    remove(filename.c_str());
    CHECK(v_trajs.size() == v_trajs_2.size());
    CHECK(tube1 == tube3);
    for(int i = 0 ; i < v_trajs.size() ; i++)
      CHECK(v_trajs[i] == v_trajs_2[i]);
  }
}

bool test_serialization(const Tube& tube1)
{
  string filename = "test_serialization.tube";

  Trajectory traj_test1a, traj_test1b, traj_test2a, traj_test2b;

  for(int i = 0 ; i < tube1.nb_slices() ; i++)
    traj_test1a.set(tube1.get_slice(i)->domain().mid(), tube1[i].is_unbounded() | tube1[i].is_empty() ? 1. : tube1[i].mid());

  for(int i = 0 ; i < tube1.nb_slices() ; i++)
    traj_test1b.set(tube1.get_slice(i)->domain().mid(), 42.);

  vector<Trajectory> v_trajs;
  v_trajs.push_back(traj_test1a);
  v_trajs.push_back(traj_test1b);
  tube1.serialize(filename, v_trajs); // serialization

  v_trajs.clear();
  Tube tube2(filename, v_trajs); // deserialization
  remove(filename.c_str());

  bool equality = tube1 == tube2;

  if(v_trajs.size() != 2)
    return false;

  traj_test2a = v_trajs[0];
  traj_test2b = v_trajs[1];
  equality &= traj_test1a == traj_test2a;
  equality &= traj_test1b == traj_test2b;

  return equality;
}

TEST_CASE("(de)serializations on bounded tubes", "[core]")
{
  SECTION("Test tube1")
  {
    CHECK(test_serialization(tube_test_1()));
  }

  SECTION("Test tube1(01)")
  {
    CHECK(test_serialization(tube_test_1_01()));    
  }

  SECTION("Test tube2")
  {
    CHECK(test_serialization(tube_test2()));
  }

  SECTION("Test tube3")
  {
    CHECK(test_serialization(tube_test3()));
  }

  SECTION("Test tube4")
  {
    CHECK(test_serialization(tube_test4()));
  }

  SECTION("Test tube4(05)")
  {
    CHECK(test_serialization(tube_test4_05()));
  }
}

TEST_CASE("(de)serializations on unbounded tubes", "[core]")
{
  SECTION("Test POS_REALS")
  {
    Tube tube = tube_test_1();
    tube.set(Interval::POS_REALS, tube.nb_slices() / 2);
    CHECK(test_serialization(tube));
    tube.set(Interval::POS_REALS);
    CHECK(test_serialization(tube));
  }

  SECTION("Test NEG_REALS")
  {
    Tube tube = tube_test2();
    tube.set(Interval::NEG_REALS, 5);
    CHECK(test_serialization(tube));
    tube.set(Interval::NEG_REALS);
    CHECK(test_serialization(tube));
  }

  SECTION("Test ALL_REALS")
  {
    Tube tube = tube_test3();
    tube.set(Interval::ALL_REALS, 1);
    CHECK(test_serialization(tube));
    tube.set(Interval::ALL_REALS);
    CHECK(test_serialization(tube));
  }

  SECTION("Test EMPTY_SET")
  {
    Tube tube = tube_test4();
    tube.set(Interval::EMPTY_SET, 0);
    tube.set(Interval::EMPTY_SET, 8);
    CHECK(test_serialization(tube));
    tube.set(Interval::EMPTY_SET);
    CHECK(test_serialization(tube));
  }
}
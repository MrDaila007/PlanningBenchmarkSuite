#include <gtest/gtest.h>
#include "geometry/kdtree2d.hpp"
#include "geometry/point2d.hpp"
#include "environment/continuous_environment.hpp"
#include "planners/prm.hpp"
#include "planners/lazy_prm.hpp"
#include "planners/rrt.hpp"
#include "planners/rrt_star.hpp"
#include "planners/informed_rrt_star.hpp"

namespace {

TEST(KdTreeTest, KNearest) {
  std::vector<pbs::Point2D> pts = {{0,0},{1,0},{2,0},{0,1},{1,1},{2,1}};
  pbs::KdTree2D tree;
  tree.build(pts);
  auto knn = tree.k_nearest(pbs::Point2D(0.5, 0.5), 3);
  EXPECT_EQ(knn.size(), 3u);
  EXPECT_EQ(tree.nearest(pbs::Point2D(0.1, 0.1)), 0u);
  EXPECT_EQ(tree.nearest(pbs::Point2D(2.1, 1.1)), 5u);
}

TEST(PRMTest, EmptySpace) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::PRMPlanner prm(200, 8);
  pbs::Path path = prm.solve(env, pbs::State(1, 1), pbs::State(9, 9));
  EXPECT_TRUE(path.success) << "PRM should find path in empty space";
  EXPECT_GT(path.states.size(), 1u);
  EXPECT_GT(path.length, 0);
}

TEST(PRMTest, WithObstacle) {
  std::vector<pbs::Point2D> sq = {{4,3},{5,3},{5,7},{4,7}};
  std::vector<pbs::Polygon> obs = {pbs::Polygon(sq)};
  pbs::ContinuousEnvironment env(0, 10, 0, 10, obs);
  pbs::PRMPlanner prm(400, 12);
  pbs::Path path = prm.solve(env, pbs::State(2, 5), pbs::State(8, 5));
  EXPECT_TRUE(path.success) << "PRM should find path around obstacle";
}

TEST(LazyPRMTest, SameResultAsPRM) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::PRMPlanner prm(150, 8);
  pbs::LazyPRMPlanner lazy(150, 8);
  pbs::Path p1 = prm.solve(env, pbs::State(2, 2), pbs::State(8, 8));
  pbs::Path p2 = lazy.solve(env, pbs::State(2, 2), pbs::State(8, 8));
  EXPECT_TRUE(p1.success);
  EXPECT_TRUE(p2.success);
  EXPECT_NEAR(p1.length, p2.length, 2.0);
}

TEST(RRTTest, EmptySpace) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::RRTPlanner rrt(1.0, 0.15, 3000);
  pbs::Path path = rrt.solve(env, pbs::State(1, 1), pbs::State(9, 9));
  EXPECT_TRUE(path.success) << "RRT should find path";
  EXPECT_GT(rrt.nodes_expanded(), 0);
}

TEST(RRTStarTest, ShorterThanRRT) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::RRTPlanner rrt(0.8, 0.1, 2000);
  pbs::RRTStarPlanner rrtstar(0.8, 0.1, 3000, 15.0);
  pbs::Path p1 = rrt.solve(env, pbs::State(2, 2), pbs::State(8, 8));
  pbs::Path p2 = rrtstar.solve(env, pbs::State(2, 2), pbs::State(8, 8));
  if (p1.success && p2.success)
    EXPECT_LE(p2.length, p1.length + 2.0) << "RRT* should be no worse than RRT";
}

TEST(InformedRRTStarTest, ConvergenceTracking) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::InformedRRTStarPlanner irrt(0.8, 0.1, 2000, 15.0);
  pbs::Path path = irrt.solve(env, pbs::State(2, 2), pbs::State(8, 8));
  if (path.success) {
    auto& cd = irrt.convergence_data();
    EXPECT_FALSE(cd.cost_vs_iteration.empty());
    for (size_t i = 1; i < cd.cost_vs_iteration.size(); ++i) {
      EXPECT_LE(cd.cost_vs_iteration[i].second, cd.cost_vs_iteration[i-1].second + 1e-6)
          << "Cost should not increase";
    }
  }
}

TEST(InformedRRTStarTest, FindsPath) {
  pbs::ContinuousEnvironment env(0, 10, 0, 10, {});
  pbs::InformedRRTStarPlanner irrt(0.8, 0.2, 2500, 12.0);
  pbs::Path path = irrt.solve(env, pbs::State(1, 1), pbs::State(9, 9));
  EXPECT_TRUE(path.success) << "Informed RRT* should find path";
}

}  // namespace

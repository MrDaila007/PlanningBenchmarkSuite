#include <gtest/gtest.h>
#include "geometry/polygon.hpp"
#include "geometry/continuous_collision_checker.hpp"
#include "geometry/point2d.hpp"
#include "environment/continuous_environment.hpp"
#include "environment/se2_environment.hpp"

namespace {

TEST(ContinuousCollisionTest, SegmentNotIntersectSquare) {
  std::vector<pbs::Point2D> sq = {{0,0},{1,0},{1,1},{0,1}};
  pbs::Polygon poly(sq);
  std::vector<pbs::Polygon> obs = {poly};
  pbs::ContinuousCollisionChecker checker(obs);
  EXPECT_FALSE(checker.segment_intersects_obstacles(
      pbs::Point2D(2,0), pbs::Point2D(2,1)));
}

TEST(ContinuousCollisionTest, SegmentIntersectsSquare) {
  std::vector<pbs::Point2D> sq = {{0,0},{1,0},{1,1},{0,1}};
  pbs::Polygon poly(sq);
  std::vector<pbs::Polygon> obs = {poly};
  pbs::ContinuousCollisionChecker checker(obs);
  EXPECT_TRUE(checker.segment_intersects_obstacles(
      pbs::Point2D(0.5,-1), pbs::Point2D(0.5,2)));
}

TEST(ContinuousCollisionTest, ClearanceOutsideObstacle) {
  std::vector<pbs::Point2D> sq = {{0,0},{1,0},{1,1},{0,1}};
  pbs::Polygon poly(sq);
  std::vector<pbs::Polygon> obs = {poly};
  pbs::ContinuousCollisionChecker checker(obs);
  double c = checker.clearance_at(pbs::Point2D(2, 2));
  EXPECT_NEAR(c, 1.414, 0.1);  // Distance to corner (1,1)
}

TEST(ContinuousCollisionTest, PointInPolygon) {
  std::vector<pbs::Point2D> tri = {{0,0},{1,0},{0.5,1}};
  pbs::Polygon poly(tri);
  EXPECT_TRUE(poly.contains(pbs::Point2D(0.5, 0.3)));
}

TEST(ContinuousEnvTest, PointInObstacle) {
  std::vector<pbs::Point2D> tri = {{0,0},{1,0},{0.5,1}};
  std::vector<pbs::Polygon> obs = {pbs::Polygon(tri)};
  pbs::ContinuousEnvironment env(0, 10, 0, 10, obs);
  pbs::State s(0.5, 0.3);
  EXPECT_FALSE(env.is_valid(s));
}

TEST(ContinuousEnvTest, CollisionFreeThroughGap) {
  std::vector<pbs::Point2D> left = {{0,0},{1,0},{1,5},{0,5}};
  std::vector<pbs::Point2D> right = {{4,0},{5,0},{5,5},{4,5}};
  std::vector<pbs::Polygon> obs = {pbs::Polygon(left), pbs::Polygon(right)};
  pbs::ContinuousEnvironment env(0, 5, 0, 5, obs);
  EXPECT_TRUE(env.collision_free(pbs::State(2, 2), pbs::State(3, 2)));
}

TEST(SE2EnvTest, BasicValidation) {
  auto base = std::make_shared<pbs::ContinuousEnvironment>(
      0, 10, 0, 10, std::vector<pbs::Polygon>{});
  pbs::SE2Environment env(base);
  pbs::State s(5, 5, 0.5);
  EXPECT_TRUE(env.is_valid(s));
}

}  // namespace

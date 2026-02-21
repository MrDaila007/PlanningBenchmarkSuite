#include <gtest/gtest.h>
#include "core/state.hpp"
#include "core/path.hpp"

namespace {

TEST(CoreTest, PathComputeLength) {
  pbs::Path path;
  path.states = {pbs::State(0, 0), pbs::State(1, 0), pbs::State(2, 0)};
  path.compute_length();
  EXPECT_DOUBLE_EQ(path.length, 2.0);
}

TEST(CoreTest, PathEmptyAndSuccess) {
  pbs::Path p;
  EXPECT_TRUE(p.empty());
  EXPECT_FALSE(p.success);
}

TEST(CoreTest, StateComparison) {
  EXPECT_EQ(pbs::State(1, 2), pbs::State(1, 2));
  EXPECT_NE(pbs::State(1, 2), pbs::State(1, 3));
}

}  // namespace

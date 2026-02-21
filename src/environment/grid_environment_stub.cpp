#include "environment/grid_environment.hpp"
#include "geometry/grid_collision_checker.hpp"
#include <nlohmann/json.hpp>

namespace pbs {

static int get_row(const State& s) {
  if (s.grid_pos)
    return s.grid_pos->first;
  return static_cast<int>(s.y);
}
static int get_col(const State& s) {
  if (s.grid_pos)
    return s.grid_pos->second;
  return static_cast<int>(s.x);
}

GridEnvironment::GridEnvironment(int width, int height)
  : width_(width), height_(height),
    occupancy_(height, std::vector<int>(width, 0)) {}

GridEnvironment::GridEnvironment(int width, int height,
                                 std::vector<std::vector<int>> occupancy)
  : width_(width), height_(height), occupancy_(std::move(occupancy)) {}

bool GridEnvironment::is_valid(const State& s) const {
  int r = get_row(s), c = get_col(s);
  return r >= 0 && r < height_ && c >= 0 && c < width_ &&
         !occupied(r, c);
}

bool GridEnvironment::collision_free(const State& a, const State& b) const {
  GridCollisionChecker checker(width_, height_, occupancy_);
  return checker.line_of_sight(get_row(a), get_col(a), get_row(b), get_col(b));
}

double GridEnvironment::clearance(const State& /*s*/) const {
  return 0.0;  // Stub
}

bool GridEnvironment::occupied(int row, int col) const {
  if (row < 0 || row >= height_ || col < 0 || col >= width_)
    return true;
  return occupancy_[row][col] != 0;
}

GridEnvironment GridEnvironment::from_json(const std::string& json) {
  auto j = nlohmann::json::parse(json);
  int w = j["width"];
  int h = j["height"];
  std::vector<std::vector<int>> occ(h);
  for (int r = 0; r < h; ++r)
    for (int el : j["occupancy"][r])
      occ[r].push_back(el);
  return GridEnvironment(w, h, std::move(occ));
}

std::string GridEnvironment::to_json() const {
  nlohmann::json j;
  j["width"] = width_;
  j["height"] = height_;
  j["occupancy"] = occupancy_;
  return j.dump();
}

}  // namespace pbs

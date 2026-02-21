#include "geometry/kdtree2d.hpp"
#include <limits>

namespace pbs {

namespace {

double dist_sq(const Point2D& a, const Point2D& b) {
  double dx = a.x - b.x, dy = a.y - b.y;
  return dx * dx + dy * dy;
}

}  // namespace

void KdTree2D::build(const std::vector<Point2D>& points) {
  points_ = points;
  indices_.resize(points_.size());
  for (size_t i = 0; i < indices_.size(); ++i) indices_[i] = i;
  build_rec(0, points_.size(), 0);
}

void KdTree2D::build_rec(size_t l, size_t r, int axis) {
  if (r - l <= 1) return;
  size_t mid = l + (r - l) / 2;
  auto cmp = [this, axis](size_t a, size_t b) {
    double va = (axis == 0) ? points_[a].x : points_[a].y;
    double vb = (axis == 0) ? points_[b].x : points_[b].y;
    return va < vb;
  };
  std::nth_element(indices_.begin() + l, indices_.begin() + mid,
                   indices_.begin() + r, cmp);
  build_rec(l, mid, 1 - axis);
  build_rec(mid + 1, r, 1 - axis);
}

size_t KdTree2D::nearest(const Point2D& p) const {
  if (points_.empty()) return 0;
  double best = 1e99;
  size_t best_i = 0;
  for (size_t i = 0; i < points_.size(); ++i) {
    double d = dist_sq(p, points_[i]);
    if (d < best) { best = d; best_i = i; }
  }
  return best_i;
}

std::vector<size_t> KdTree2D::k_nearest(const Point2D& p, size_t k) const {
  if (points_.empty()) return {};
  k = std::min(k, points_.size());
  std::vector<std::pair<double, size_t>> heap;
  for (size_t i = 0; i < points_.size(); ++i) {
    double d = dist_sq(p, points_[i]);
    heap.push_back({d, i});
  }
  std::partial_sort(heap.begin(), heap.begin() + static_cast<ptrdiff_t>(k),
                   heap.end());
  std::vector<size_t> result;
  for (size_t i = 0; i < k; ++i) result.push_back(heap[i].second);
  return result;
}

std::vector<size_t> KdTree2D::radius_search(const Point2D& p, double r) const {
  std::vector<size_t> result;
  double r2 = r * r;
  for (size_t i = 0; i < points_.size(); ++i) {
    if (dist_sq(p, points_[i]) <= r2)
      result.push_back(i);
  }
  return result;
}

}  // namespace pbs

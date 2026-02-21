#pragma once

#include <vector>
#include <cmath>

namespace pbs {

inline double mean(const std::vector<double>& v) {
  if (v.empty()) return 0;
  double s = 0;
  for (double x : v) s += x;
  return s / v.size();
}

inline double std_dev(const std::vector<double>& v) {
  if (v.size() < 2) return 0;
  double m = mean(v);
  double sum_sq = 0;
  for (double x : v) sum_sq += (x - m) * (x - m);
  return std::sqrt(sum_sq / (v.size() - 1));
}

std::pair<double, double> confidence_interval_95(const std::vector<double>& v);

}  // namespace pbs

#include "benchmark/statistics.hpp"
#include <cmath>

namespace pbs {

std::pair<double, double> confidence_interval_95(const std::vector<double>& v) {
  if (v.size() < 2) return {mean(v), mean(v)};
  double m = mean(v);
  double s = std_dev(v);
  double n = static_cast<double>(v.size());
  double t = 1.96;
  if (n <= 30) {
    static const double t_table[] = {
      12.71, 4.30, 3.18, 2.78, 2.57, 2.45, 2.36, 2.31, 2.26, 2.23,
      2.20, 2.18, 2.16, 2.14, 2.13, 2.12, 2.11, 2.10, 2.09, 2.09,
      2.08, 2.07, 2.07, 2.06, 2.06, 2.06, 2.05, 2.05, 2.05, 2.04
    };
    int idx = std::min(static_cast<int>(n) - 1, 29);
    t = t_table[idx];
  }
  double err = t * s / std::sqrt(n);
  return {m - err, m + err};
}

}  // namespace pbs

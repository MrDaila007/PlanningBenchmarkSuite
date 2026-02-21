/**
 * Charts for Planning Benchmark Suite: metrics bar charts, convergence line chart.
 * Uses Chart.js (loaded from CDN).
 */

const Charts = {
  _instances: {},

  /** Create or update metrics bar chart */
  metricsBar(canvasId, results, metric = 'mean_path_length') {
    const canvas = document.getElementById(canvasId);
    if (!canvas || !window.Chart) return null;
    const labels = results.map(r => r.planner);
    const values = results.map(r => r[metric] ?? 0);

    if (this._instances[canvasId]) this._instances[canvasId].destroy();
    this._instances[canvasId] = new Chart(canvas, {
      type: 'bar',
      data: {
        labels,
        datasets: [{
          label: metric.replace(/_/g, ' '),
          data: values,
          backgroundColor: 'rgba(52, 152, 219, 0.7)',
          borderColor: 'rgba(52, 152, 219, 1)',
          borderWidth: 1,
        }],
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: { legend: { display: false } },
        scales: {
          y: { beginAtZero: true },
        },
      },
    });
    return this._instances[canvasId];
  },

  /** Create or update convergence line chart */
  convergence(canvasId, data) {
    const canvas = document.getElementById(canvasId);
    if (!canvas || !window.Chart) return null;
    const pts = Array.isArray(data) ? data : (data?.convergence || data?.cost_vs_iteration || []);
    if (!pts.length) return null;

    const labels = pts.map(p => p[0]);
    const values = pts.map(p => p[1]);

    if (this._instances[canvasId]) this._instances[canvasId].destroy();
    this._instances[canvasId] = new Chart(canvas, {
      type: 'line',
      data: {
        labels,
        datasets: [{
          label: 'Best cost',
          data: values,
          borderColor: 'rgba(46, 204, 113, 1)',
          backgroundColor: 'rgba(46, 204, 113, 0.1)',
          fill: true,
          tension: 0.2,
        }],
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: { legend: { display: false } },
        scales: {
          x: { title: { display: true, text: 'Iteration' } },
          y: { title: { display: true, text: 'Cost' }, beginAtZero: true },
        },
      },
    });
    return this._instances[canvasId];
  },

  destroyAll() {
    Object.values(this._instances).forEach(c => c && c.destroy());
    this._instances = {};
  },
};

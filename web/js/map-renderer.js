/**
 * Map renderer + path overlay for Planning Benchmark Suite.
 * Renders occupancy grid and path on a canvas.
 */

const MapRenderer = {
  /** @param {HTMLCanvasElement} canvas */
  render(canvas, options = {}) {
    const { environment, path, start, goal } = options;
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#f8f9fa';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    if (!environment || !environment.occupancy) return;
    const ctx = canvas.getContext('2d');
    const w = environment.width;
    const h = environment.height;
    const occ = environment.occupancy;

    const rect = canvas.getBoundingClientRect();
    const scale = Math.min(rect.width / w, rect.height / h, 32);
    const offX = (rect.width - w * scale) / 2;
    const offY = (rect.height - h * scale) / 2;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#f8f9fa';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.save();
    ctx.translate(offX, offY);
    ctx.scale(scale, scale);

    for (let r = 0; r < h; r++) {
      for (let c = 0; c < w; c++) {
        ctx.fillStyle = (occ[r] && occ[r][c]) ? '#2c3e50' : '#ecf0f1';
        ctx.fillRect(c, r, 1, 1);
      }
    }

    if (path && path.states && path.states.length > 1) {
      ctx.strokeStyle = '#3498db';
      ctx.lineWidth = 0.25;
      ctx.lineCap = 'round';
      ctx.beginPath();
      const pts = path.states;
      ctx.moveTo(pts[0].x, pts[0].y);
      for (let i = 1; i < pts.length; i++) ctx.lineTo(pts[i].x, pts[i].y);
      ctx.stroke();
    }

    if (start) {
      ctx.fillStyle = '#27ae60';
      ctx.beginPath();
      ctx.arc(start[0], start[1], 0.4, 0, Math.PI * 2);
      ctx.fill();
    }
    if (goal) {
      ctx.fillStyle = '#e74c3c';
      ctx.beginPath();
      ctx.arc(goal[0], goal[1], 0.4, 0, Math.PI * 2);
      ctx.fill();
    }

    ctx.restore();
  },

  /** Resize canvas to match display size and re-render */
  setupCanvas(canvas, options) {
    const rect = canvas.getBoundingClientRect();
    const dpr = window.devicePixelRatio || 1;
    if (canvas.width !== rect.width * dpr || canvas.height !== rect.height * dpr) {
      canvas.width = rect.width * dpr;
      canvas.height = rect.height * dpr;
      const ctx = canvas.getContext('2d');
      ctx.scale(dpr, dpr);
    }
    this.render(canvas, options);
  },
};

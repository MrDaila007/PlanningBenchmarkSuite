/**
 * Dijkstra planner — 8-connected grid, pure JS port.
 */

(function (global) {
  const DR = [-1, 1, 0, 0, -1, -1, 1, 1];
  const DC = [0, 0, -1, 1, -1, 1, -1, 1];
  const COST = [1, 1, 1, 1, Math.SQRT2, Math.SQRT2, Math.SQRT2, Math.SQRT2];
  const N = 8;

  function getRow(s) { return Math.floor(s.y); }
  function getCol(s) { return Math.floor(s.x); }

  function solve(env, start, goal) {
    let nodesExpanded = 0;
    const w = env.width, h = env.height;
    const occ = env.occupancy;
    const gridOcc = (r, c) => r < 0 || r >= h || c < 0 || c >= w || (occ[r] && occ[r][c]);
    const collisionFree = typeof GridEnvironment !== 'undefined'
      ? (a, b) => GridEnvironment.collisionFree(env, a, b)
      : () => true;

    let sr = getRow(start), sc = getCol(start);
    let gr = getRow(goal), gc = getCol(goal);

    if (gridOcc(sr, sc) || gridOcc(gr, gc)) {
      return { path: { states: [], success: false, length: 0 }, nodes_expanded: 0 };
    }

    const pq = [];
    const parent = {};
    const best = {};
    const key = (r, c) => r + ',' + c;
    best[key(sr, sc)] = 0;
    pq.push({ g: 0, r: sr, c: sc });

    const cmp = (a, b) => a.g - b.g;

    while (pq.length > 0) {
      pq.sort(cmp);
      const { g, r, c } = pq.shift();
      const k = key(r, c);
      if (best[k] !== undefined && best[k] < g - 1e-9) continue;
      nodesExpanded++;

      if (r === gr && c === gc) {
        const states = [];
        let rc = { r, c };
        while (rc) {
          states.push({ x: rc.c, y: rc.r });
          rc = parent[key(rc.r, rc.c)];
        }
        states.reverse();
        let length = 0;
        for (let i = 1; i < states.length; i++) {
          length += Math.hypot(states[i].x - states[i - 1].x, states[i].y - states[i - 1].y);
        }
        return {
          path: { states, success: true, length },
          nodes_expanded: nodesExpanded
        };
      }

      for (let i = 0; i < N; i++) {
        const nr = r + DR[i], nc = c + DC[i];
        if (nr < 0 || nr >= h || nc < 0 || nc >= w) continue;
        if (gridOcc(nr, nc)) continue;
        if (!collisionFree({ x: c, y: r }, { x: nc, y: nr })) continue;
        const ng = g + COST[i];
        const nk = key(nr, nc);
        if (best[nk] !== undefined && best[nk] <= ng) continue;
        best[nk] = ng;
        parent[nk] = { r, c };
        pq.push({ g: ng, r: nr, c: nc });
      }
    }

    return { path: { states: [], success: false, length: 0 }, nodes_expanded: nodesExpanded };
  }

  global.DijkstraPlanner = { solve };
})(typeof window !== 'undefined' ? window : this);

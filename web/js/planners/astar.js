/**
 * A* planner — 8-connected grid, diagonal heuristic, pure JS port.
 */

(function (global) {
  const DR = [-1, 1, 0, 0, -1, -1, 1, 1];
  const DC = [0, 0, -1, 1, -1, 1, -1, 1];
  const COST = [1, 1, 1, 1, Math.SQRT2, Math.SQRT2, Math.SQRT2, Math.SQRT2];
  const N = 8;

  function getRow(s) { return Math.floor(s.y); }
  function getCol(s) { return Math.floor(s.x); }

  function heuristicDiagonal(r0, c0, r1, c1) {
    const dr = Math.abs(r1 - r0), dc = Math.abs(c1 - c0);
    return Math.max(dr, dc) + (Math.SQRT2 - 1) * Math.min(dr, dc);
  }

  function solve(env, start, goal) {
    let nodesExpanded = 0;
    const w = env.width, h = env.height;
    const occ = env.occupancy;
    const gridOcc = (r, c) => r < 0 || r >= h || c < 0 || c >= w || (occ[r] && occ[r][c]);
    const collisionFree = typeof GridEnvironment !== 'undefined'
      ? (a, b) => GridEnvironment.collisionFree(env, a, b)
      : () => true;

    const sr = getRow(start), sc = getCol(start);
    const gr = getRow(goal), gc = getCol(goal);

    if (gridOcc(sr, sc) || gridOcc(gr, gc)) {
      return { path: { states: [], success: false, length: 0 }, nodes_expanded: 0 };
    }

    const hFunc = (r, c) => heuristicDiagonal(r, c, gr, gc);

    const pq = [];
    const parent = {};
    const gBest = {};
    const key = (r, c) => r + ',' + c;
    gBest[key(sr, sc)] = 0;
    pq.push({ f: hFunc(sr, sc), r: sr, c: sc });

    const cmp = (a, b) => a.f - b.f;

    while (pq.length > 0) {
      pq.sort(cmp);
      const { f, r, c } = pq.shift();
      const g = gBest[key(r, c)];
      if (g + 1e-9 < f - hFunc(r, c)) continue;
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
        if (gBest[nk] !== undefined && gBest[nk] <= ng) continue;
        gBest[nk] = ng;
        parent[nk] = { r, c };
        pq.push({ f: ng + hFunc(nr, nc), r: nr, c: nc });
      }
    }

    return { path: { states: [], success: false, length: 0 }, nodes_expanded: nodesExpanded };
  }

  global.AStarPlanner = { solve };
})(typeof window !== 'undefined' ? window : this);

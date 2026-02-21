/**
 * MapGenerator — pure JS port of C++ MapGenerator.
 * Generates random_uniform and maze (Kruskal) occupancy grids.
 */

(function (global) {
  function mulberry32(seed) {
    let s = (seed >>> 0) & 0xFFFFFFFF;
    return function () {
      let t = (s += 0x6D2B79F5) >>> 0;
      t = Math.imul(t ^ (t >>> 15), t | 1);
      t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
      return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
    };
  }

  function shuffleArray(arr, rng) {
    const a = arr.slice();
    for (let i = a.length - 1; i > 0; i--) {
      const j = Math.floor(rng() * (i + 1));
      [a[i], a[j]] = [a[j], a[i]];
    }
    return a;
  }

  const MapGenerator = {
    generate(params) {
      const { width, height, type = 'random_uniform', obstacle_density = 0.2, seed = 42 } = params;
      if (type === 'random_uniform') {
        return this.randomUniform(width, height, obstacle_density, seed);
      }
      if (type === 'maze') {
        return this.maze(width, height, seed);  // width/height = cells for maze
      }
      throw new Error('Unknown type: ' + type);
    },

    randomUniform(width, height, density, seed) {
      const rng = mulberry32(seed || 42);
      const occ = [];
      for (let r = 0; r < height; r++) {
        const row = [];
        for (let c = 0; c < width; c++) {
          if (r === 0 && c === 0) {
            row.push(0);
            continue;
          }
          if (r === height - 1 && c === width - 1) {
            row.push(0);
            continue;
          }
          row.push(rng() < density ? 1 : 0);
        }
        occ.push(row);
      }
      return {
        type: 'grid',
        width,
        height,
        occupancy: occ
      };
    },

    maze(cellsWide, cellsHigh, seed) {
      const rng = mulberry32(seed || 42);
      const gridH = 2 * cellsHigh + 1;
      const gridW = 2 * cellsWide + 1;

      const occ = [];
      for (let r = 0; r < gridH; r++) {
        const row = [];
        for (let c = 0; c < gridW; c++) {
          row.push(1);
        }
        occ.push(row);
      }
      for (let r = 0; r < cellsHigh; r++) {
        for (let c = 0; c < cellsWide; c++) {
          occ[2 * r + 1][2 * c + 1] = 0;
        }
      }

      const edges = [];
      for (let r = 0; r < cellsHigh; r++) {
        for (let c = 0; c < cellsWide; c++) {
          const idx = r * cellsWide + c;
          if (r + 1 < cellsHigh) edges.push([idx, (r + 1) * cellsWide + c]);
          if (c + 1 < cellsWide) edges.push([idx, r * cellsWide + (c + 1)]);
        }
      }

      const shuffled = shuffleArray(edges, rng);

      const parent = [];
      const rank = [];
      for (let i = 0; i < cellsWide * cellsHigh; i++) {
        parent[i] = i;
        rank[i] = 0;
      }

      function find(x) {
        if (parent[x] !== x) parent[x] = find(parent[x]);
        return parent[x];
      }

      function unite(a, b) {
        a = find(a);
        b = find(b);
        if (a === b) return;
        if (rank[a] < rank[b]) parent[a] = b;
        else if (rank[a] > rank[b]) parent[b] = a;
        else {
          parent[b] = a;
          rank[a]++;
        }
      }

      for (const [a, b] of shuffled) {
        if (find(a) !== find(b)) {
          unite(a, b);
          const ra = Math.floor(a / cellsWide);
          const ca = a % cellsWide;
          const rb = Math.floor(b / cellsWide);
          const cb = b % cellsWide;
          if (rb === ra + 1) {
            occ[2 * ra + 2][2 * ca + 1] = 0;
          } else {
            occ[2 * ra + 1][2 * ca + 2] = 0;
          }
        }
      }

      return {
        type: 'grid',
        width: gridW,
        height: gridH,
        occupancy: occ
      };
    }
  };

  global.MapGenerator = MapGenerator;
})(typeof window !== 'undefined' ? window : this);

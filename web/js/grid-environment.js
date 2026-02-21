/**
 * GridEnvironment — collision checking for occupancy grids.
 * Port of C++ GridCollisionChecker (Bresenham line_of_sight).
 */

(function (global) {
  function isOccupied(occupancy, width, height, row, col) {
    if (row < 0 || row >= height || col < 0 || col >= width) return true;
    return occupancy[row] && occupancy[row][col] !== 0;
  }

  function lineOfSight(occupancy, width, height, r0, c0, r1, c1) {
    const dr = Math.abs(r1 - r0);
    const dc = Math.abs(c1 - c0);
    const sr = r0 < r1 ? 1 : -1;
    const sc = c0 < c1 ? 1 : -1;
    let r = r0, c = c0;

    if (dc >= dr) {
      let err = 2 * dr - dc;
      for (let i = 0; i <= dc; i++) {
        if (isOccupied(occupancy, width, height, r, c)) return false;
        if (r === r1 && c === c1) return true;
        if (err > 0) {
          r += sr;
          err -= 2 * dc;
        }
        err += 2 * dr;
        c += sc;
      }
    } else {
      let err = 2 * dc - dr;
      for (let i = 0; i <= dr; i++) {
        if (isOccupied(occupancy, width, height, r, c)) return false;
        if (r === r1 && c === c1) return true;
        if (err > 0) {
          c += sc;
          err -= 2 * dr;
        }
        err += 2 * dc;
        r += sr;
      }
    }
    return true;
  }

  const GridEnvironment = {
    occupied(env, row, col) {
      return isOccupied(env.occupancy, env.width, env.height, row, col);
    },

    isValid(env, state) {
      const row = Math.floor(state.y);
      const col = Math.floor(state.x);
      return !this.occupied(env, row, col);
    },

    collisionFree(env, a, b) {
      const r0 = Math.floor(a.y), c0 = Math.floor(a.x);
      const r1 = Math.floor(b.y), c1 = Math.floor(b.x);
      return lineOfSight(env.occupancy, env.width, env.height, r0, c0, r1, c1);
    }
  };

  global.GridEnvironment = GridEnvironment;
})(typeof window !== 'undefined' ? window : this);

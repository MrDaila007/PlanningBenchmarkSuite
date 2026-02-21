/**
 * Parser for Planning Benchmark Suite data formats.
 * Supports: *_results.json (benchmark), *_results.csv, visualization JSON (env+path).
 */

const Parser = {
  /** @returns {Object} { format: 'benchmark'|'visualization'|'unknown', data } */
  parse(text, filename = '') {
    const trimmed = text.trim();
    if (trimmed.startsWith('{')) return this._parseJson(text);
    if (trimmed.startsWith('planner,') || /^[a-z_]+,[\d.]+,[\d.]+/.test(trimmed)) return this._parseCsv(text);
    return { format: 'unknown', data: null };
  },

  _parseJson(text) {
    let data;
    try {
      data = JSON.parse(text);
    } catch (e) {
      return { format: 'unknown', data: null, error: e.message };
    }
    if (data.type === 'visualization' && data.environment && data.path !== undefined) {
      return { format: 'visualization', data };
    }
    if (Array.isArray(data.results) && data.results.length > 0) {
      const r = data.results[0];
      if (r.planner !== undefined && (r.mean_path_length !== undefined || r.mean_time_ms !== undefined)) {
        return { format: 'benchmark', data };
      }
    }
    return { format: 'unknown', data };
  },

  _parseCsv(text) {
    const lines = text.split(/\r?\n/).filter(Boolean);
    if (lines.length < 2) return { format: 'unknown', data: null };
    const header = lines[0].toLowerCase();
    if (!header.includes('planner') || !header.includes('mean_')) return { format: 'unknown', data: null };

    const cols = lines[0].split(',').map(s => s.trim().toLowerCase());
    const idx = (name) => cols.indexOf(name);
    const results = [];
    for (let i = 1; i < lines.length; i++) {
      const vals = lines[i].split(',').map(s => s.trim());
      const get = (name) => {
        const i = idx(name);
        const v = i >= 0 ? vals[i] : '';
        return typeof v === 'string' ? v.replace(/^"|"$/g, '') : v;
      };
      results.push({
        planner: get('planner') || `run_${i}`,
        mean_path_length: parseFloat(get('mean_path_length')) || 0,
        std_path_length: parseFloat(get('std_path_length')) || 0,
        mean_time_ms: parseFloat(get('mean_time_ms')) || 0,
        std_time_ms: parseFloat(get('std_time_ms')) || 0,
        mean_nodes: parseFloat(get('mean_nodes')) || 0,
        success_rate: parseFloat(get('success_rate')) || 0,
        ci_low: parseFloat(get('ci_low')) || null,
        ci_high: parseFloat(get('ci_high')) || null,
      });
    }
    return { format: 'benchmark', data: { results } };
  },
};

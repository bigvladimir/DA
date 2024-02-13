#include <iostream>
#include <vector>
#include <set>

using namespace std;

using ll = long long;
const ll INF = 1e18;

struct rebro {
  int v;
  ll w;
};

using graph = vector<vector<rebro>>;

bool Bellman_Ford(const int s, vector<ll> &d, const graph &g) {
  const int n = g.size();
  d = vector<ll>(n, INF);
  d[s] = 0;
  int iter = 0;
  bool changed = true;
  while(changed && iter < n) {
    changed = false;
    for(int i = 0; i < n; ++i) {
      if (d[i] == INF) continue;
      for(const rebro &r : g[i]) {
        if (d[r.v] > d[i] + r.w) {
          d[r.v] = d[i] + r.w;
          changed = true;
        }
      }
    }
    ++iter;
  }
  if (iter == n && changed) return false;
  return true;
}

void Dijkstra(const int s, vector<ll> &d, const graph &g) {
  const int n = g.size();
  d = vector<ll>(n, INF);
  vector<bool> visited(n);
  set<pair<ll, int>> sub_pq;
  d[s] = 0;
  sub_pq.insert({0, s});
  while(!sub_pq.empty()) {
    auto temp_ptr = sub_pq.begin();
    int u = (*temp_ptr).second;
    sub_pq.erase(temp_ptr);
    if (visited[u]) continue;
    visited[u] = true;
    for(const rebro &r : g[u]) {
      if (d[r.v] > d[u] + r.w) {
        d[r.v] = d[u] + r.w;
        sub_pq.insert({d[r.v], r.v});
      }
    }
  }
}


//если хочется сохранить начальный граф то нужно передавать не по ссылке или делать обратное перевзвешивание
bool Johnson(vector<vector<ll>> &dd, graph g) {
  const int n = g.size();
  dd = vector<vector<ll>>(n, vector<ll>(n, INF));
  g.push_back(vector<rebro>(n));
  for(int i = 0; i < n; ++i) g[n][i] = {i, 0};
  vector<ll> d;
  if (Bellman_Ford(n, d, g) == false) return false;
  for(int i = 0; i < n; ++i) {
    for(rebro &r : g[i]) {
      r.w = r.w + d[i] - d[r.v];
    }
  }
  vector<ll> temp_d;
  for(int i = 0; i < n; ++i) {
    Dijkstra(i, temp_d, g);
    for(int j = 0; j < n; ++j) {
      if (temp_d[j] == INF) continue;
      dd[i][j] = temp_d[j] - d[i] + d[j];
    }
  }
  return true;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n, m;
  cin >> n >> m;
  graph g(n);
  int u, v;
  ll w;
  for(int i = 0; i < m; ++i) {
    cin >> u >> v >> w;
    --u;
    --v;
    g[u].push_back({v, w});
  }

  vector<vector<ll>> dd;
  if (Johnson(dd, g)) {
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < n; ++j) {
        if (dd[i][j] == INF) {
          cout << "inf" << ' ';
        } else {
          cout << dd[i][j] << ' ';
        }
      }
      cout << '\n';
    }
  } else {
    cout << "Negative cycle";
  }

  return 0;
}
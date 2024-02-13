#include <iostream>
#include <vector>
#include <set>

using namespace std;

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  
  int m, n;
  cin >> m >> n;

  vector<vector<float>> tab(m, vector<float>(n + 2));
  for(int i = 0; i < m; ++i) {
    for(int j = 0; j < n; ++j) {
      cin >> tab[i][j];
    }
    tab[i][n] = i + 1;
    cin >> tab[i][n + 1];
  }

  set<int> result;
  for(int i = 0; i < n; ++i) {
    pair<int, int> s_c(-1, 1000);
    for(int j = i; j < m; ++j)
      if (tab[j][i] != 0 && tab[j][n + 1] < s_c.second)
        s_c = {j, tab[j][n + 1]};
    if (s_c.first == -1) {
      cout << -1;
      return 0;
    }
    swap(tab[s_c.first], tab[i]);
    for(int j = i + 1; j < m; ++j)
      for(pair<int, float> k(i, tab[j][i]/tab[i][i]); k.first < n; ++k.first)
        tab[j][k.first] -= k.second * tab[i][k.first];
    result.insert(tab[i][n]);
  }

  for(auto &r : result)
    cout << r << ' ';
  
  return 0;
}
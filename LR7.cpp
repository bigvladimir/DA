#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
  int n;
  cin >> n;

  vector<pair<int, int>> dp(n+1);
  for(int i = 2; i <= n; ++i) {
    dp[i].first = dp[i-1].first + i;
    if (i % 2 == 0)
      dp[i] = min(dp[i], {dp[i / 2].first + i, 2});
    if (i % 3 == 0)
      dp[i] = min(dp[i], {dp[i / 3].first + i, 3});
  }
  
  cout << dp[n].first << '\n';
  for(int i = n; i > 1;) {
    if (dp[i].second == 0) {
      cout << "-1 ";
      --i;
    } else {
      cout << '/' << dp[i].second << ' ';
      i /= dp[i].second;
    }
  }

  return 0;
}
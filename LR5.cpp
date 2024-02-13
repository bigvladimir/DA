#include <iostream>
#include <vector>
#include <set>

using namespace std;

const int ALP_SIZE = 27;
constexpr char END_CHAR = 'z' + 1;

class Suff_tree {
private:
  struct Node {
    int left, right, pos;
    vector<Node*> child;

    Node(int t_l, int t_r, int t_p) : left(t_l), right(t_r), pos(t_p) {
      child.resize(ALP_SIZE);
    }
  };

  string text;
  Node *root;

  inline void insert(Node* node, int left, const int pos) {
    int index, shift = 0;
    Node *cur = node;
    
    do {
      node = cur;
      left += shift;
      index = text[left] - 'a';
      
      if (node->child[index] == nullptr) {
        node->child[index] = new Node(left, text.length() - 1, pos);
        return;
      }
      
      cur = node->child[index];
      shift = 0;
      while(cur->left + shift <= cur->right &&
            text[left + shift] == text[cur->left + shift]) {
        ++shift;
      }
    } while (cur->left + shift > cur->right);

    Node *new_child = new Node(cur->left, cur->left + shift - 1, -1);
    cur->left += shift;
    new_child->child[text[cur->left] - 'a'] = cur;
    new_child->child[text[left + shift] - 'a'] =
      new Node(left + shift, text.length() - 1, pos);
    node->child[index] = new_child;
  }

  void every_pos(Node* node, set<int> &answ) {
    if (node->pos == -1) {
      for(int i = 0; i < ALP_SIZE; ++i)
        if (node->child[i] != nullptr)
          every_pos(node->child[i], answ);
    } else {
      answ.insert(node->pos);
    }
  }

public:
  Suff_tree(const string &t) : text(t + END_CHAR) {
    root = new Node(0, 0, 0);
  }

  void fill() {
    for (int i = 0; i < ALP_SIZE; ++i) {
      if (root->child[i] != nullptr) {
        return;
      }
    }
    for (int i = 0; i < text.length(); ++i) {
      insert(root, i, i);
    }
  }

  set<int> find(string &p) {
    Node* cur = root;
    bool flag = true;
    for(int last = 0; flag && last < p.size();) {
      cur = cur->child[p[last] - 'a'];
      if (cur) {
        for(int i = cur->left; cur->right >= i && last < p.size(); ++i) {
          if (p[last] != text[i]) break;
          ++last;
        }
      }
      flag = cur && !(cur->right >= cur->left + last && last < p.size());
    }
    set<int> answ;
    if (flag) every_pos(cur, answ);
    return answ;
  }
};


int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  string text;
  cin >> text;

  Suff_tree tree(text);

  tree.fill();

  string p;
  bool is_first;
  for(int count = 1; cin >> p; ++count) {
    set<int> answ = tree.find(p);
    is_first = true;
    for(int i : answ) {
      if (is_first) {
        cout << count << ": " << i + 1;
        is_first = false;
      } else {
        cout << ", " << i + 1;
      }
    }
    if (!is_first) cout << '\n';
  }

  return 0;
}
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <random>
#include <chrono>
#include <string.h>

using namespace std;

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

using ull = unsigned long long;



class Treap {
private:
  struct Node;
  struct Treap_pair;

  Node *root;

  void kill(Node *treap_pointer);
  Treap_pair split(Node *treap, char *key);
  Node *merge(Treap_pair p);
  Node **search_remove(Node **treap, char *key);
  Node *search(Node *treap, char *key);
public:
  Treap();
  ~Treap();

  bool insert(char *key, ull value);

  bool remove(char *key);

  struct find_output;
  find_output find(char *key);
};



struct Treap::Node {
  char *key;
  uint_fast32_t priority;
  ull value;
  Node *left, *right;
  Node(char *_key, ull _value, uint_fast32_t _priority, Treap_pair p);
  ~Node();
};

struct Treap::Treap_pair {
  Node *left, *right;
};

Treap::Node::Node(char *_key, ull _value, uint_fast32_t _priority, Treap_pair p) {
  key = new char[257];
  for(int i = 0; i < 257; ++i) key[i] = _key[i];
  priority = _priority;
  value = _value;
  left = p.left;
  right = p.right;
}

Treap::Node::~Node() {
  delete[] key;
}

struct Treap::find_output {
  bool result;
  ull value;
};



Treap::Treap() {
  root = nullptr;
}

Treap::Treap_pair Treap::split(Node *treap, char *key) {
  if (treap == nullptr) {
    return {nullptr, nullptr};
  } else if (strcmp(key, treap->key) > 0) {
    Treap_pair t = split(treap->right, key);
    treap->right = t.left;
    return {treap, t.right};
  } else {
    Treap_pair t = split(treap->left, key);
    treap->left = t.right;
    return {t.left, treap};
  }
}

Treap::Node *Treap::merge(Treap_pair p) {
  if (p.right == nullptr) {
    return p.left;
  } else if (p.left == nullptr) {
    return p.right;
  } else if (p.left->priority > p.right->priority) {
    p.left->right = merge({p.left->right, p.right});
    return p.left;
  } else {
    p.right->left = merge({p.left, p.right->left});
    return p.right;
  }
}

bool Treap::insert(char *key, ull value) {
  uint_fast32_t priority = rng();
  Node **t_treap = &root;
  while(*t_treap != nullptr && (*t_treap)->priority >= priority) { // !!! >
    int result = strcmp(key, (*t_treap)->key);
    if (result < 0) {
      t_treap = &(*t_treap)->left;
    } else if (result > 0) {
      t_treap = &(*t_treap)->right;
    } else {
      return false;
    }
  }
  Node *try_treap = *t_treap;
  while(try_treap != nullptr) {
    int result = strcmp(key, try_treap->key);
    if (result < 0) {
      try_treap = try_treap->left;
    } else if (result > 0) {
      try_treap = try_treap->right;
    } else {
      return false;
    }
  }
  *t_treap = new Node(key, value, priority, split(*t_treap, key));
  return true;
}

Treap::Node **Treap::search_remove(Node **treap, char *key) {
  while(*treap != nullptr) {
    int result = strcmp(key, (*treap)->key);
    if (result < 0) {
      treap = &(*treap)->left;
    } else if (result > 0) {
      treap = &(*treap)->right;
    } else {
      return treap;
    }
  }
  return treap;
}

bool Treap::remove(char *key) {
  Node **p = search_remove(&root, key);

  if (*p != nullptr) {
    Node *n_p = merge({(*p)->left, (*p)->right});
    delete *p;
    *p = n_p;
    
    return true;
  } else {
    return false;
  }
}

Treap::Node *Treap::search(Node *treap, char *key) {
  while(treap != nullptr) {
    int result = strcmp(key, treap->key);
    if (result < 0) {
      treap = treap->left;
    } else if (result > 0){
      treap = treap->right;
    } else {
      return treap;
    }
  }
  return treap;
}

Treap::find_output Treap::find(char *key) {
  Node *result = search(root, key);
  if (result != nullptr) {
    return {true, result->value};
  } else {
    return {false, 0};
  }
}

void Treap::kill(Node *treap_pointer) {
  if (treap_pointer) {
    kill(treap_pointer->left);
    kill(treap_pointer->right);
    delete treap_pointer;
  }
}

Treap::~Treap() {
  kill(root);
}



char *to_lower(char *input) {
  for (size_t i = 0; input[i] != '\0'; ++i)
    input[i] = tolower(input[i]);
  return input;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  
  Treap *treap = new Treap;

  char input[257];
  Treap::find_output find_result;
  ull value;

  while (std::cin >> input) {
    to_lower(input);
    if (input[0] == '+') {
      std::cin >> input >> value;
      if (treap->insert(to_lower(input), value)) {
        std::cout << "OK\n";
      } else {
        std::cout << "Exist\n";
      }
    } else if (input[0] == '-') {
      std::cin >> input;
      if (treap->remove(to_lower(input))) {
        std::cout << "OK\n";
      } else {
        std::cout << "NoSuchWord\n";
      }
    } else {
      find_result = treap->find(input);
      if (find_result.result) {
        std::cout << "OK: " << find_result.value << '\n';
      } else {
        std::cout << "NoSuchWord\n";
      }
    }
  }

  delete treap;

  return 0;
}

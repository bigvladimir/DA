#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

using ull = unsigned long long;



class Treap {
private:
  struct Node;
  struct Treap_pair;
  struct remove_info;

  Node *root;

  void kill(Node *treap_pointer);
  Treap_pair split(Node *treap, char *key);
  Node *merge(Treap_pair p);
  Node *search(Node *treap, char *key);
  remove_info search_remove(Node *treap, char *key);
public:
  Treap();
  ~Treap();

  int insert(char *key, ull value);

  int remove(char *key);

  struct find_output;
  find_output find(char *key);
};



struct Treap::Node {
  char *key;
  int priority;
  ull value;
  Node *left, *right;
  Node(char *input_key, ull input_value);
  ~Node();
};

Treap::Node::Node(char *input_key, ull input_value) {
  key = new char[257];
  for(int i = 0; i < 257; ++i) key[i] = input_key[i];
  priority = rand();
  value = input_value;
  left = nullptr;
  right = nullptr;
}

Treap::Node::~Node() {
  delete[] key;
}

struct Treap::Treap_pair {
  Node *left, *right;
};

struct Treap::remove_info {
  Node *res, *prev;
  int l;
};

struct Treap::find_output {
  int result;
  ull value;
};



Treap::Treap() {
  root = nullptr;
}

Treap::Treap_pair Treap::split(Node *treap, char *key) {
  if (!treap) {
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
  if (!p.right) {
    return p.left;
  } else if (!p.left) {
    return p.right;
  } else if (p.left->priority > p.right->priority) {
    p.left->right = merge({p.left->right, p.right});
    return p.left;
  } else {
    p.right->left = merge({p.left, p.right->left});
    return p.right;
  }
}

Treap::Node *Treap::search(Node *treap, char *key) {
  while(treap) {
    int result = strcmp(key, treap->key);
    if (result == 0) {
      return treap;
    } else if (result < 0) {
      treap = treap->left;
    } else {
      treap = treap->right;
    }
  }
  return treap;
}

int Treap::insert(char *key, ull value) {
  if (!search(root, key)) {
    Treap_pair p = split(root, key);
    p.left = merge({p.left, new Node(key, value)});
    root = merge(p);
    return 1;
  } else {
    return 0;
  }
}

Treap::remove_info Treap::search_remove(Node *treap, char *key) {
  Node *prev = nullptr;
  int l;
  while(treap) {
    int result = strcmp(key, treap->key);
    if (result == 0) {
      return {treap, prev, l};
    } else if (result < 0) {
      prev = treap;
      l = 1;
      treap = treap->left;
    } else {
      prev = treap;
      l = 0;
      treap = treap->right;
    }
  }
  return {treap, prev, l};
}

int Treap::remove(char *key) {
  remove_info p = search_remove(root, key);
  
  if (p.res) {
    if (p.prev) {
      Node *n_p = merge({p.res->left, p.res->right});
      if (p.l) {
        delete p.res;
        p.prev->left = n_p;
      } else {
        delete p.res;
        p.prev->right = n_p;
      }
    } else {
      Node *n_p = merge({p.res->left, p.res->right});
      delete root;
      root = n_p;
    }
    return 1;
  } else {
    return 0;
  }
}

Treap::find_output Treap::find(char *key) {
  Node *result = search(root, key);
  if (result) {
    return {1, result->value};
  } else {
    return {0, 0};
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
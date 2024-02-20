#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;



struct node {
  int key;
  node *left, *right;
  int nodeCount, rightCount; //считать сколько в левой ветке нам не надо
  //конструктор для первичного построения дерева
  node(int _key) : key(_key), left(nullptr), right(nullptr),
    nodeCount(0), rightCount(0) {}
  //конструктор копирования не копирующий детей
  node(node *t_node) : key(t_node->key), left(nullptr), right(nullptr),
    nodeCount(t_node->nodeCount), rightCount(t_node->rightCount) {}
};

struct event {
  bool isItStart; //начало и конец отрезка
  int x, y;
  node *tree; //хранение ссылки на персистентную ветку
};



struct persistentBinaryTree {
  int n, nn; //количество отрезков, количество событий = n*2
  vector<event> events;

  //построение начального пустого дерева
  void buildBaseTree(node *&curNode, int lBoard, int rBoard) {
    if (lBoard == rBoard) {
      return;
    }
    int lMedian = (rBoard + lBoard) / 2;
    int rMedian = lMedian + 1;
    curNode = new node(events[lMedian].y);
    //нужно не пустить в дерево одинаковые игрики
    while (lMedian > 1 && events[lMedian - 1].y == events[lMedian].y) {
      --lMedian;
    }
    while (rMedian <= nn && events[rMedian].y == events[rMedian - 1].y) {
      ++rMedian;
    }
    buildBaseTree(curNode->left, lBoard, lMedian);
    buildBaseTree(curNode->right, rMedian, rBoard);
  }


  //insert и remove строят новую ветвь идя по старой
  //при подсчете нам важно только количетсво отрезков справа
  //то есть выше запросного игрика
  void insertBranch(node *oldNode, node *&curNode, int y) {
    /*if (oldNode == nullptr) {
      return;
    }*/
    //такое условие никогда не выполнится ибо все высоты есть
    curNode = new node(oldNode);
    if (y < oldNode->key) {
      curNode->right = oldNode->right;
      insertBranch(oldNode->left, curNode->left, y);
    } else if (y > oldNode->key) {
      ++(curNode->rightCount);
      curNode->left = oldNode->left;
      insertBranch(oldNode->right, curNode->right, y);
    } else {
      ++(curNode->nodeCount);
      curNode->left = oldNode->left;
      curNode->right = oldNode->right;
    }
  }

  void removeBranch(node *oldNode, node *&curNode, int y) {
    curNode = new node(oldNode);
    if (y < oldNode->key) {
      curNode->right = oldNode->right;
      removeBranch(oldNode->left, curNode->left, y);
    } else if (y > oldNode->key) {
      --(curNode->rightCount);
      curNode->left = oldNode->left;
      removeBranch(oldNode->right, curNode->right, y);
    } else {
      --(curNode->nodeCount);
      curNode->left = oldNode->left;
      curNode->right = oldNode->right;
    }
  }

  //в зависимости от того начала или конец отрезка
  //вызывает функцию insert или remove новой ветки
  void buildPersistenBranch(int event) {
    if (events[event].isItStart) {
      insertBranch(events[event - 1].tree, events[event].tree, events[event].y);
    } else {
      removeBranch(events[event - 1].tree, events[event].tree, events[event].y);
    }
  }

  static bool compX (const event &a, const event &b) {
    return a.x < b.x;
  }

  static bool compY (const event &a, const event &b) {
    return a.y < b.y;
  }

  void fill() {
    //желательно написать сортировки за линеное время
    //хотя от этого асимптотика алгоритма не улучшится
    //ибо уже создание всех персистентных веток вместе за nlogn

    //сортируем по игрику чтобы построить начальное дерево по ним
    sort(events.begin() + 1, events.end(), compY);
    buildBaseTree(events[0].tree, 1, nn + 1);

    //сортируем по икс чтобы построить персистентные корни
    sort(events.begin() + 1, events.end(), compX);
    for (int i = 1; i <= nn; ++i) {
      buildPersistenBranch(i);
    }

    //устранение случаев с кучей одинаковых икс, нужен только самый правый
    //для этого в более левых одинаковых икс делаю указатель на более правую ветку
    int prevX = events[nn].x;
    for (int i = nn - 1; i >= 1; --i) {
      if (prevX == events[i].x) {
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //утечка памяти !!!!!!!!!!!!!!!!!!!!!
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //но в принципе не критично, так как
        //она ограничена своим размером, а не растет постоянно
        events[i].tree = events[i + 1].tree;
      } else {
        prevX = events[i].x;
      }
    }
  }

  //проход по версии дерева до нужного нам y или nullptr
  int countForRequest(node *curNode, int y) {
    int counter = 0;
    while(curNode != nullptr && y != curNode->key) {
      if (y < curNode->key) {
        //если спускаемся влево добавляем правую ветку и то что в этой ноде
        counter += curNode->rightCount + curNode->nodeCount;
        curNode = curNode->left;
      } else {
        curNode = curNode->right;
      }
    }
    //если не nullptr, значит остановились в какой-то точке
    //и надо добавить ее правую ветку
    if (curNode != nullptr) {
      counter += curNode->rightCount;
    }
    return counter;
  }



  //переделать бы в класс и запихнуть функции ниже в паблик

  persistentBinaryTree(int _n) : n(_n), nn(n*2) {
    //нужно в два раза больше места для событий тк храним начало и конец,
    //еще одно место для базового дерева
    events.resize(nn + 1); //nn = n*2

    //в нулевой точке будет храниться дерево со всеми высотами и нулевыми значениями
    events[0] = {true, -2000000000, 0, nullptr};
  }

  //вызывается после конструктора, можно запихнуть в конструктор
  void scanAndFill() {
    int t_l, t_r, t_y;
    for (int i = 1; i <= nn; ++i) {
      cin >> t_l >> t_r >> t_y;
      events[i] = {true, t_l, t_y, nullptr};
      ++i;
      //сдвигаю конец на единицу, так как может быть ошибка, если мы запросим точку
      //в которой отрезок закончился
      events[i] = {false, t_r + 1, t_y, nullptr};
    }
    fill();
  }

  //хештег делать запросы
  int request(int x, int y) {
    //бинарным поиском ищем где x
    //поиск кривой, надо функцию фулл переписать
    //если код не работает, то скорее всего ошибка в бинпоиске

    int curX = n, lb = 1, rb = nn; //n - середина, тк nn = n*2
    while(rb - lb >= 0) {
      if (x < events[curX].x) {
        rb = curX - 1;
      } else {
        lb = curX + 1;
      }
      curX = (lb+rb) / 2;
    }

    if (curX >= 1 && curX < nn) { // последний элемент nn нет смысла пересчитывать, там может быть только конец отрезка
      return countForRequest(events[curX].tree, y);
    } else {
      return 0;
    }
  }
};



int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n, m;
  cin >> n >> m;

  persistentBinaryTree tree(n);

  tree.scanAndFill();

  int t_x, t_y;
  for(int i = 0; i < m; ++i) {
    cin >> t_x >> t_y;
    cout << tree.request(t_x, t_y) << '\n';
  }

  return 0;
}

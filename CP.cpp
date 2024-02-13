#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

using vectors = vector<vector<int>>;

const int MAX_D = 10;
constexpr int MAX_D_PLUS = MAX_D + 1;


class KDtree {
private:
    struct Node {
        double key[MAX_D_PLUS];
        Node* left, * right;
        Node(const vector<int>& t_key) : left(nullptr), right(nullptr) {
            for (int i = 0; i < t_key.size(); ++i) {
                key[i] = t_key[i];
            }
        }
    };

    const int D;
    Node* root;

    double min_dist, nearest;

    struct t_comp {
        int index;
        inline bool operator ()
            (const vector<int>& a, const vector<int>& b) {
            return a[index] < b[index];
        }
    } comp;

    void fill(vectors& data, const int lb, const int rb, Node*& node, const int depth) {
        if (lb >= rb) {
            return;
        }

        const int t_dim = depth % D;

        comp.index = t_dim;
        sort(data.begin() + lb, data.begin() + rb, comp);

        int median = (lb + rb) / 2;

        while(median < rb - 1 && data[median][t_dim] == data[median + 1][t_dim]) {
            ++median;
        }

        node = new Node(data[median]);

        fill(data, lb, median, node->left, depth + 1);
        fill(data, median + 1, rb, node->right, depth + 1);
    }

    void near(const Node* node, const int depth, const double(&p)[MAX_D]) {
        if (node == nullptr) {
            return;
        }

        const int t_dim = depth % D;

        double t_dist = 0, t_m;
        for (int i = 0; i < D; ++i) {
            t_m = p[i] - node->key[i];
            t_dist += t_m * t_m;
        }

        if (t_dist < min_dist) {
            nearest = node->key[D];
            min_dist = t_dist;
        }

        const double t_key = node->key[t_dim];
        const double t_p = p[t_dim];
        const double t_dif = (t_p - t_key) * (t_p - t_key);

        if (t_p > t_key) {
            near(node->right, depth + 1, p);
            if (t_dif < min_dist) {
                near(node->left, depth + 1, p);
            }
        } else {
            near(node->left, depth + 1, p);
            if (t_dif < min_dist) {
                near(node->right, depth + 1, p);
            }
        }
    }


public:
    KDtree(vectors& data, const int dim) : D(dim), root(nullptr) {
        fill(data, 0, data.size(), root, 0);
    }

    int find_neighbor(const double(&p)[MAX_D]) {
        nearest = -1;
        min_dist = 1e18;
        near(root, 0, p);
        return (int)nearest + 1;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, d, t_d, q;

    cin >> n >> d;

    t_d = d + 1;
    vectors data(n, vector<int>(t_d));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < d; ++j) {
            cin >> data[i][j];
        }
        data[i][d] = i;
    }

    KDtree kdtree(data, d);

    cin >> q;

    double p[MAX_D];

    for (int i = 0; i < q; ++i) {
        for (int j = 0; j < d; ++j) {
            cin >> p[j];
        }
        cout << kdtree.find_neighbor(p) << '\n';
    }

    return 0;
}
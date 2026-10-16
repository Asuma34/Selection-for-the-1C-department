#include <iostream>
#include <vector>

struct Edge {
  int from;
  int to;
  long long len;
  long long deadline;
  std::vector<int> req; // requirements
};

struct Art {
  Art (int where, int cost, int from, int to) : where(where), cost(cost),
                                                from(from), to(to) {}
  int where;
  int cost;
  int from;
  int to;
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  
  int T_lim;
  double T_cost;
  std::cin >> T_lim >> T_cost;
  
  int in;
  int num_out;
  std::cin >> in >> num_out;
  std::vector<int> out(num_out);
  for (int i = 0; i < num_out; ++i) {
    std::cin >> out[i];
  }

  std::vector<Edge> edge(m);
  for (int i = 0; i < m; ++i) {
    int req_count;
    std::cin >> edge[i].from >> edge[i].to >> edge[i].len >> edge[i].deadline;
    std::cin >> req_count;
    edge[i].req.resize(req_count);
    for (int j = 0; j < req_count; ++j) {
      std::cin >> edge[i].req[j];
    }
  }

  int k;
  std::cin >> k;
  std::vector<Art> arts;
  arts.reserve(k);
  for (int i = 0; i < k; ++i) {
    int where;
    int cost;
    int from;
    int to;
    std::cin >> where >> cost >> from >> to;
    arts.emplace_back(where, cost, from, to);
  }


  return 0;
}

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

const long long cInf = 1e16;

template <typename T>
void Swap(T& a, T& b) {
  T t = a;
  a = b;
  b = t;
}

class Heap {
 public:
  Heap() = default;
  size_t GetMinFirst() const { return heap_[0].first; }
  long long GetMinSecond() const { return heap_[0].second; }

  bool Empty() const { return heap_.empty(); }

  void SiftUp(long long ind) {
    long long pos = (ind - 1) / 2;
    if (ind > 0 && heap_[ind].second < heap_[pos].second) {
      Swap(heap_[ind].first, heap_[pos].first);
      Swap(heap_[ind].second, heap_[pos].second);
      SiftUp(pos);
    }
  }

  void SiftDown(long long ind) {
    long long ind_min = ind;
    if (ind * 2 + 1 < (long long)heap_.size() &&
        heap_[ind * 2 + 1].second < heap_[ind_min].second) {
      ind_min = ind * 2 + 1;
    }
    if (ind * 2 + 2 < (long long)heap_.size() &&
        heap_[ind * 2 + 2].second < heap_[ind_min].second) {
      ind_min = ind * 2 + 2;
    }
    if (ind == ind_min) {
      return;
    }
    Swap(heap_[ind].first, heap_[ind_min].first);
    Swap(heap_[ind].second, heap_[ind_min].second);
    SiftDown(ind_min);
  }

  void Insert(size_t id, long long x) {
    heap_.push_back({id, x});
    SiftUp(heap_.size() - 1);
  }

  void ExtractMin() {
    Swap(heap_[0].first, heap_[heap_.size() - 1].first);
    Swap(heap_[0].second, heap_[heap_.size() - 1].second);
    heap_.pop_back();
    if (!heap_.empty()) {
      SiftDown(0);
    }
  }

  void DecreaseKey(long long ind, long long delta) {
    heap_[ind].second -= delta;
    SiftUp(ind);
  }

 private:
  std::vector<std::pair<size_t, long long>> heap_;
};

struct Edge {
  int from;
  int to;
  long long len;
  long long deadline;
  std::vector<int> req; // requirements
  size_t need = 0;
};

struct Art {
  Art(int where, int cost, long long from, long long to)
      : where(where), cost(cost), from(from), to(to) {}
  int where;
  int cost;
  long long from;
  long long to;
};

struct Mission {
  int n;
  int m;
  int k;
  long long T_lim;
  double T_cost;
  int in;
  std::vector<int> out;
  std::vector<Edge> edge;
  std::vector<Art> arts;
  std::vector<std::vector<int>> next;
  std::vector<std::vector<int>> prev;
  std::vector<std::vector<int>> art_at;
};

Mission Input() {
  Mission mission;
  std::cin >> mission.n >> mission.m;
  std::cin >> mission.T_lim >> mission.T_cost;

  int num_out;
  std::cin >> mission.in >> num_out;
  mission.out.resize(num_out);
  for (int i = 0; i < num_out; ++i) {
    std::cin >> mission.out[i];
  }

  mission.edge.resize(mission.m);
  for (int i = 0; i < mission.m; ++i) {
    int req_count;
    std::cin >> mission.edge[i].from >> mission.edge[i].to
             >> mission.edge[i].len >> mission.edge[i].deadline;
    std::cin >> req_count;
    mission.edge[i].req.resize(req_count);
    for (int j = 0; j < req_count; ++j) {
      std::cin >> mission.edge[i].req[j];
    }
  }

  std::cin >> mission.k;
  mission.arts.reserve(mission.k);
  for (int i = 0; i < mission.k; ++i) {
    int where;
    int cost;
    long long from;
    long long to;
    std::cin >> where >> cost >> from >> to;
    mission.arts.emplace_back(where, cost, from, to);
  }

  return mission;
}

void Prepare(Mission& mission) {
  mission.next.assign(mission.n, std::vector<int>());
  mission.prev.assign(mission.n, std::vector<int>());
  mission.art_at.assign(mission.n, std::vector<int>());

  for (int i = 0; i < mission.m; ++i) {
    Edge& e = mission.edge[i];
    mission.next[e.from].push_back(i);
    mission.prev[e.to].push_back(i);
    e.need = 0;
    for (int id : e.req) {
      e.need |= 1ULL << id;
    }
  }

  for (int i = 0; i < mission.k; ++i) {
    mission.art_at[mission.arts[i].where].push_back(i);
  }
}

std::vector<std::vector<long long>> Dijkstra(const Mission& mission,
                                           long long start_time) {
  size_t mask_count = 1ULL << mission.k;
  std::vector<std::vector<long long>> res(
      mission.n, std::vector<long long>(mask_count, cInf));
  if (start_time > mission.T_lim) {
    return res;
  }

  res[mission.in][0] = start_time;
  std::vector<std::vector<bool>> expanded(
      mission.n, std::vector<bool>(mask_count, false));
  Heap a;
  a.Insert(mission.in, start_time);

  while (true) {
    if (a.Empty()) {
      break;
    }
    size_t id = a.GetMinFirst();
    long long curr_dist = a.GetMinSecond();
    a.ExtractMin();

    size_t v = id % mission.n;
    size_t mask = id / mission.n;
    if (expanded[v][mask] || curr_dist > res[v][mask]) {
      continue;
    }
    expanded[v][mask] = true;

    for (int index : mission.next[v]) {
      const Edge& e = mission.edge[index];
      if ((mask & e.need) != e.need) {
        continue;
      }
      long long new_dist = curr_dist + e.len;
      if (new_dist > e.deadline || new_dist > mission.T_lim) {
        continue;
      }
      size_t to = e.to;
      if (new_dist < res[to][mask]) {
        res[to][mask] = new_dist;
        a.Insert(mask * mission.n + to, new_dist);
      }
    }

    for (int index : mission.art_at[v]) {
      size_t bit = 1ULL << index;
      if ((mask & bit) != 0) {
        continue;
      }
      const Art& art = mission.arts[index];
      long long new_dist = std::max(curr_dist, art.from);
      if (new_dist > art.to || new_dist > mission.T_lim) {
        continue;
      }
      size_t new_mask = mask | bit;
      if (new_dist < res[v][new_mask]) {
        res[v][new_mask] = new_dist;
        a.Insert(new_mask * mission.n + v, new_dist);
      }
    }
  }

  return res;
}

int main() {
  Mission mission = Input();
  Prepare(mission);
  std::vector<std::vector<long long>> dist = Dijkstra(mission, 0);
  return 0;
}

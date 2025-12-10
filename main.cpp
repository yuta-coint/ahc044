#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// --- 提供されたクラス群 ---

// 時間をDouble型で管理し、経過時間も取り出せるクラス
class TimeKeeperDouble {
 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  double time_threshold_;

  double now_time_ = 0;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeperDouble(const double time_threshold)
      : start_time_(std::chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // 経過時間をnow_time_に格納する。
  void setNowTime() {
    auto diff = std::chrono::high_resolution_clock::now() - this->start_time_;
    this->now_time_ =
        std::chrono::duration_cast<std::chrono::microseconds>(diff).count() *
        1e-3;  // ms
  }

  // 経過時間をnow_time_に取得する。
  double getNowTime() const { return this->now_time_; }

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const { return now_time_ >= time_threshold_; }
};

// 乱数を生成するクラス
class Random {
 public:
  std::mt19937 mt_;  // シード0でメルセンヌツイスターの乱数生成器を初期化
  // 0以上1.0未満の実数の範囲の乱数生成
  uniform_real_distribution<double> dd_{0, 1.0};

  // seedを指定して初期化
  Random(const int seed = 0) : mt_(std::mt19937(seed)) {}

  // 0以上m未満の整数の範囲の乱数
  inline int nextInt(const int m) {
    uniform_int_distribution<int> di(0, m - 1);
    return di(mt_);
  }

  // 0以上1.0未満の実数の範囲の乱数
  inline double nextDouble() { return dd_(mt_); }

  // 0以上1.0未満の実数の範囲の乱数のlog。焼きなまし法で使いやすい。
  inline double nextLog() { return log(dd_(mt_)); }
};

// --- 実装本体 ---

// グローバル変数
int N;
ll L;
vector<ll> T;
vector<int> tuika;
Random rnd; // グローバルインスタンス

// calc関数: スコアと差分(sabun)を計算する
ll calc(const vector<pair<int, int>>& G, vector<ll>& sab) {
    vector<ll> work_count(N, 0);
    for (int i = 0; i < N; ++i) {
        work_count[G[i].first] += T[i];
        work_count[G[i].second] += T[i];
    }

    ll score = 0;
    for (int i = 0; i < N; ++i) {
        ll diff = work_count[i] - T[i] * 2;
        score += abs(diff);
        sab[i] = diff;
    }
    return score;
}

// 共通処理: tuikaリストの作成
void prepare_tuika() {
    priority_queue<pair<ll, int>> pq;
    for (int i = 0; i < N; ++i) {
        pq.push({T[i], i});
    }

    tuika.reserve(N);
    for (int i = 0; i < N; ++i) {
        auto top = pq.top(); pq.pop();
        ll val = top.first;
        int idx = top.second;
        
        tuika.push_back(idx);
        pq.push({val - 2500, idx});
    }
}

// 初期解生成関数 (Phase 1)
vector<pair<int, int>> generate_initial_solution(vector<pair<int, int>>& final_ans, ll& ideal_score) {
    vector<ll> sab(N); 

    for (int sana = 0; sana < 80; ++sana) {
        // 初期グラフ構築
        vector<pair<int, int>> ans(N);
        for (int i = 0; i < N; ++i) {
            int u = (i + sana + 1) % N;
            int idx = (i - sana) % N;
            if (idx < 0) idx += N;
            int v = tuika[idx];
            ans[i] = {u, v};
        }

        // 簡易探索 (Phase 1は回数固定)
        for (int iter = 0; iter < 1000; ++iter) {
            ll sc = calc(ans, sab);
            if (sc < ideal_score) {
                final_ans = ans;
                ideal_score = sc;
            }

            // ターゲット選定
            int hataraki = rnd.nextInt(N);
            int sabori = rnd.nextInt(N);
            int cut = rnd.nextInt(N);

            for (int k = 0; k < N; ++k) {
                int i = (cut + k) % N;
                if (sab[i] < sab[sabori]) {
                    if (rnd.nextInt(100) < 95) sabori = i;
                }
                if (sab[i] > sab[hataraki]) {
                    if (rnd.nextInt(100) < 95) hataraki = i;
                }
            }

            // 変更適用
            cut = rnd.nextInt(N);
            bool found = false;
            for (int k = 0; k < N; ++k) {
                int i = (cut + k) % N;
                if (ans[i].first == hataraki) {
                    ans[i].first = sabori;
                    found = true;
                } else if (ans[i].second == hataraki) {
                    ans[i].second = sabori;
                    found = true;
                }
                if (found) break;
            }
        }
    }
    return final_ans;
}

// 局所探索関数 (Phase 2) - 時間制限対応
void run_local_search(vector<pair<int, int>>& ans, ll& ideal_score, vector<pair<int, int>>& final_ans, TimeKeeperDouble& tk) {
    vector<ll> sab(N);
    ll sc = calc(ans, sab);
    ll loop_cnt = 0;

    // 時間いっぱいループする
    while (true) {
        loop_cnt++;
        // 毎回時間を取得すると重いので、128回に1回チェックする
        if ((loop_cnt & 127) == 0) {
            tk.setNowTime();
            if (tk.isTimeOver()) break;
        }

        if (sc < ideal_score) {
            final_ans = ans;
            ideal_score = sc;
        }

        int hataraki = rnd.nextInt(N);
        int sabori = rnd.nextInt(N);
        int cut = rnd.nextInt(N);

        for (int k = 0; k < N; ++k) {
            int i = (cut + k) % N;
            if (sab[i] < sab[sabori]) {
                if (rnd.nextInt(100) < 95) sabori = i;
            }
            if (sab[i] > sab[hataraki]) {
                if (rnd.nextInt(100) < 95) hataraki = i;
            }
        }

        int t_i = -1, t_j = -1, t_old = -1;
        bool found = false;

        cut = rnd.nextInt(N);
        for (int k = 0; k < N; ++k) {
            int i = (cut + k) % N;
            if (ans[i].first == hataraki) {
                t_i = i; t_j = 0; t_old = ans[i].first;
                ans[i].first = sabori;
                found = true;
            } else if (ans[i].second == hataraki) {
                t_i = i; t_j = 1; t_old = ans[i].second;
                ans[i].second = sabori;
                found = true;
            }
            if (found) break;
        }

        if (!found) continue;

        vector<ll> new_sab(N);
        ll new_sc = calc(ans, new_sab);

        bool improved = (sc > new_sc);
        bool noise = (rnd.nextInt(100) >= 95);

        if (improved ^ noise) {
            // 採用
            sc = new_sc;
            sab = new_sab;
        } else {
            // 棄却
            if (t_j == 0) ans[t_i].first = t_old;
            else ans[t_i].second = t_old;
        }
    }
}

int main() {
    // タイムキーパー開始 (制限時間 1950 ms)
    TimeKeeperDouble tk(1950.0);

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 乱数初期化 (ランダムシード)
    unsigned int seed = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    rnd = Random(seed);

    if (!(cin >> N >> L)) return 0;
    T.resize(N);
    for (int i = 0; i < N; ++i) cin >> T[i];

    prepare_tuika();

    vector<pair<int, int>> final_ans(N);
    for (int i = 0; i < N; ++i) final_ans[i] = {(i + 1) % N, (i + 2) % N};
    ll ideal_score = 1e14;

    // Phase 1 (回数固定)
    generate_initial_solution(final_ans, ideal_score);

    // Phase 2 (時間制限まで)
    vector<pair<int, int>> current_ans = final_ans;
    run_local_search(current_ans, ideal_score, final_ans, tk);

    for (int i = 0; i < N; ++i) {
        cout << final_ans[i].first << " " << final_ans[i].second << "\n";
    }

    return 0;
}
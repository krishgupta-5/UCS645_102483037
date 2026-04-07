#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

static inline int score(char a, char b){ return (a==b) ? 2 : -1; }

int main(){
    string A = "ACACACTA";
    string B = "AGCACACA";
    int n = (int)A.size(), m = (int)B.size();
    int gap = -2;

    int maxT = omp_get_max_threads();

    cout << "\nQ2 Smith-Waterman (Wavefront)\n";
    cout << "Threads\tTime(s)\tSpeedup\tEfficiency\tBestScore\n";

    double T1 = 0.0;

    for(int t=1; t<=maxT; t++){
        omp_set_num_threads(t);

        vector<int> H((n+1)*(m+1), 0);
        auto idx = [&](int i,int j){ return i*(m+1)+j; };
        int best = 0;
        double start = omp_get_wtime();
        for(int d=2; d<=n+m; d++){
            int iStart = max(1, d-m);
            int iEnd   = min(n, d-1);

            int diagBest = 0;

            #pragma omp parallel for schedule(static) reduction(max:diagBest)
            for(int i=iStart; i<=iEnd; i++){
                int j = d - i;

                int match = H[idx(i-1,j-1)] + score(A[i-1], B[j-1]);
                int del   = H[idx(i-1,j)] + gap;
                int ins   = H[idx(i,j-1)] + gap;

                int val = max(0, max(match, max(del, ins)));
                H[idx(i,j)] = val;

                if(val > diagBest) diagBest = val;
            }

            if(diagBest > best) best = diagBest;
        }

        double end = omp_get_wtime();
        double Tp = end - start;

        if(t==1) T1 = Tp;

        double speedup = T1 / Tp;
        double efficiency = speedup / t;

        cout << t << "\t"
             << Tp << "\t"
             << speedup << "\t"
             << efficiency << "\t\t"
             << best << "\n";
    }

    return 0;
}

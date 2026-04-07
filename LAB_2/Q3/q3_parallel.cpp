#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

int main(){
    int NX = 600, NY = 600, STEPS = 150;
    int maxT = omp_get_max_threads();

    auto idx = [&](int i,int j){ return i*NY + j; };

    cout << "\nQ3 Heat Diffusion (2D)\n";
    cout << "Threads\tTime(s)\tSpeedup\tEfficiency\tCenterValue\n";

    double T1 = 0.0;

    for(int t=1; t<=maxT; t++){
        omp_set_num_threads(t);

        // Fresh grids per run (fair timing)
        vector<double> cur(NX*NY, 0.0), nxt(NX*NY, 0.0);

        // hot spot
        for(int i=NX/2-5;i<=NX/2+5;i++)
            for(int j=NY/2-5;j<=NY/2+5;j++)
                cur[idx(i,j)] = 100.0;

        double start = omp_get_wtime();

        for(int step=0; step<STEPS; step++){
            #pragma omp parallel for schedule(static)
            for(int i=1; i<NX-1; i++){
                for(int j=1; j<NY-1; j++){
                    nxt[idx(i,j)] = 0.25 * (
                        cur[idx(i-1,j)] + cur[idx(i+1,j)] +
                        cur[idx(i,j-1)] + cur[idx(i,j+1)]
                    );
                }
            }
            cur.swap(nxt);
        }

        double end = omp_get_wtime();
        double Tp = end - start;

        if(t==1) T1 = Tp;

        double speedup = T1 / Tp;
        double efficiency = speedup / t;

        double center = cur[idx(NX/2, NY/2)];

        cout << t << "\t"
             << Tp << "\t"
             << speedup << "\t"
             << efficiency << "\t\t"
             << center << "\n";
    }

    return 0;
}
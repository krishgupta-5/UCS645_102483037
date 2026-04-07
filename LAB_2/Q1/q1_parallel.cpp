#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

struct Vec3 { double x,y,z; };

int main(){
    int N = 1200;
    double sigma=1.0, eps=1.0, rc=2.5, rc2=rc*rc;

    vector<Vec3> pos(N);
    srand(1);
    for(int i=0;i<N;i++){
        pos[i] = {(rand()/(double)RAND_MAX)*50,
                  (rand()/(double)RAND_MAX)*50,
                  (rand()/(double)RAND_MAX)*50};
    }

    int maxT = omp_get_max_threads();

    cout << "\nThreads \t Time(s) \t Speedup \t Efficiency\n";

    double T1 = 0;
    for(int t=1; t<=maxT; t++){

        omp_set_num_threads(t);

        vector<Vec3> F(N,{0,0,0});
        double E = 0;

        double start = omp_get_wtime();

        #pragma omp parallel for reduction(+:E) schedule(dynamic,8)
        for(int i=0;i<N;i++){
            for(int j=i+1;j<N;j++){
                double dx=pos[i].x-pos[j].x;
                double dy=pos[i].y-pos[j].y;
                double dz=pos[i].z-pos[j].z;

                double r2 = dx*dx+dy*dy+dz*dz;
                if(r2==0 || r2>rc2) continue;

                double invr2=1.0/r2;
                double s2=(sigma*sigma)*invr2;
                double s6=s2*s2*s2;
                double s12=s6*s6;

                E+=4*eps*(s12-s6);

                double fmag=24*eps*invr2*(2*s12-s6);

                #pragma omp atomic
                F[i].x += dx*fmag;
                #pragma omp atomic
                F[j].x -= dx*fmag;
            }
        }

        double end = omp_get_wtime();
        double Tp = end-start;

        if(t==1) T1 = Tp;

        double speedup = T1/Tp;
        double efficiency = speedup/t;

        cout << t << "\t"
             << Tp << "\t"
             << speedup << "\t"
             << efficiency << "\n";
    }
}

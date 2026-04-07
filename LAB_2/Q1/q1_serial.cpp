#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

struct Vec3 { double x,y,z; };

int main(){
    int N = 1000;
    double sigma=1.0, eps=1.0, rc=2.5, rc2=rc*rc;

    vector<Vec3> pos(N), F(N, {0,0,0});
    srand(1);
    for(int i=0;i<N;i++){
        pos[i] = { (rand()/(double)RAND_MAX)*50,
                   (rand()/(double)RAND_MAX)*50,
                   (rand()/(double)RAND_MAX)*50 };
    }

    double start = omp_get_wtime();
    double E = 0;

    for(int i=0;i<N;i++){
        for(int j=i+1;j<N;j++){
            double dx=pos[i].x-pos[j].x, dy=pos[i].y-pos[j].y, dz=pos[i].z-pos[j].z;
            double r2 = dx*dx + dy*dy + dz*dz;
            if(r2==0 || r2>rc2) continue;

            double invr2 = 1.0/r2;
            double s2 = (sigma*sigma)*invr2;
            double s6 = s2*s2*s2;
            double s12 = s6*s6;

            E += 4*eps*(s12 - s6);

            double fmag = 24*eps*invr2*(2*s12 - s6);
            double fx = dx*fmag, fy = dy*fmag, fz = dz*fmag;

            F[i].x += fx; F[i].y += fy; F[i].z += fz;
            F[j].x -= fx; F[j].y -= fy; F[j].z -= fz;
        }
    }

    double end = omp_get_wtime();
    cout << "Q1 Serial  Time=" << (end-start) << " Energy=" << E << "\n";
    cout << "F0=(" << F[0].x << "," << F[0].y << "," << F[0].z << ")\n";
    return 0;
}
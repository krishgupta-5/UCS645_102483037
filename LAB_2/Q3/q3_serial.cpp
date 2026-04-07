#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

int main(){
    int NX=600, NY=600, STEPS=150;
    vector<double> cur(NX*NY,0), nxt(NX*NY,0);
    auto idx=[&](int i,int j){ return i*NY+j; };

    for(int i=NX/2-5;i<=NX/2+5;i++)
        for(int j=NY/2-5;j<=NY/2+5;j++)
            cur[idx(i,j)] = 100.0;

    double start=omp_get_wtime();

    for(int t=0;t<STEPS;t++){
        for(int i=1;i<NX-1;i++){
            for(int j=1;j<NY-1;j++){
                nxt[idx(i,j)] = 0.25*( cur[idx(i-1,j)] + cur[idx(i+1,j)]
                                     + cur[idx(i,j-1)] + cur[idx(i,j+1)] );
            }
        }
        cur.swap(nxt);
    }

    double end=omp_get_wtime();
    cout<<"Q3 Serial Time="<<(end-start)<<" Center="<<cur[idx(NX/2,NY/2)]<<"\n";
    return 0;
}
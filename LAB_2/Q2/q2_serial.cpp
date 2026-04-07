#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

int score(char a,char b){ return (a==b)?2:-1; }

int main(){
    string A="ACACACTA", B="AGCACACA";
    int n=A.size(), m=B.size(), gap=-2;

    vector<vector<int>> H(n+1, vector<int>(m+1,0));

    double start=omp_get_wtime();
    int best=0;

    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            int match = H[i-1][j-1] + score(A[i-1],B[j-1]);
            int del   = H[i-1][j] + gap;
            int ins   = H[i][j-1] + gap;
            int val   = max(0, max(match, max(del,ins)));
            H[i][j]=val;
            best=max(best,val);
        }
    }

    double end=omp_get_wtime();
    cout<<"Q2 Serial Time="<<(end-start)<<" BestScore="<<best<<"\n";
    return 0;
}
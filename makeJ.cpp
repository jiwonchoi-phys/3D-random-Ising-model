#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

using namespace std;

int main(int argc, char **argv)
{
    int L,L2,sampleidx;
    double g;
    L = atoi(argv[1]);
    sampleidx = atoi(argv[2]);
    L2 = L*L;
    char confile[80];
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> uni(0,1);
    exponential_distribution<> expdist(2);
    
    double **J = new double*[L2];
    for (int i=0;i<L2;++i) J[i] = new double[4];

    for (int i=0;i<L2;++i){
        J[i][0] = uni(gen);
        J[i][3] = uni(gen);
        J[(i%L == L-1 ? i-L+1 : i+1)][1] = J[i][0];
        J[(i/L == L-1 ? i+L-L2 : i+L)][2] = J[i][3];
    }


    sprintf(confile,"sample%d/Jconfig",sampleidx);
    ofstream coupling;
    coupling.open(confile);
    coupling.precision(16);
    for (int i=0;i<L2;++i){
        for (int j=0;j<4;++j) coupling << J[i][j] << "\n";   
    }
    coupling.close();

    sprintf(confile,"sample%d/Gconfig",sampleidx);
    coupling.open(confile);
    coupling.precision(16);
    for (int i=0;i<L2;++i){
        coupling << expdist(gen) << "\n";
    }
    coupling.close();


    return 0;
}

#include <cstdlib>
#include <cmath>
#include <random>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> uni(0,1);

void MakeStructure(int L,int Lt,int **nn)
{
	// 0:right, 1:left, 2:up, 3:down, 4:above, 5:below
	for (int i=0;i<L*L*Lt;++i){
		nn[i][0] = (i%L == L-1 ? i-L+1 : i+1);
		nn[i][1] = (i%L == 0 ? i+L-1 : i-1);
		nn[i][2] = ((i%(L*L))/L == 0 ? i + L*L - L : i - L);
		nn[i][3] = ((i%(L*L))/L == L-1 ? i + L - L*L : i + L);
        nn[i][4] = (i/(L*L) == 0 ? i- L*L + L*L*Lt : i-L*L);
        nn[i][5] = (i/(L*L) == Lt-1 ? i + L*L - L*L*Lt : i+L*L);
    }
}

void WolffStep(int L2,int Lt,double **padd,int *spin,int **nn,int *stack)
{
	int n,ix, oldspin,newspin,current;
	int sp = 1;
	double p;
	ix = (L2*Lt)*uni(gen);
    stack[0] = ix;
	oldspin = spin[ix];	
	newspin = -oldspin;
	spin[ix] = newspin;
	
	while(sp){
		current = stack[--sp];
        for (n=0;n<6;++n){
			p = uni(gen);
			if (spin[nn[current][n]] == oldspin && p<padd[current%L2][n]){
				stack[sp++] = nn[current][n];
				spin[nn[current][n]] = newspin;
			}
		}
	}
}	

double Magnetization(int L2, int Lt, int *spin)
{
	double mag=0;
	for (int i=0;i<L2*Lt;++i) mag += (double)spin[i];
	return mag/(L2*Lt);  
}
	

int main(int argc, char **argv)
{
	int i,j,k,L,L2,Lt;
	int eq, count, measure;	
	int EQstep,Measurestep,Nmeasure;
    int FIRST, sampleidx;
    double T,beta;
	double mr,m,m2,m4;
	char dSfile[80],confile[80];
    if (argc<8){
		cout << "Usage: ./wolff L Lt T EQstep Measurestep Nmeasure sampleidx FIRST\n";
		exit(1);
	}
	L = atoi(argv[1]);
    Lt = atoi(argv[2]);
	T = atof(argv[3]);
	EQstep = atoi(argv[4]);
	Measurestep = atoi(argv[5]);
	Nmeasure = atoi(argv[6]);
    sampleidx=atoi(argv[7]);
    FIRST=atoi(argv[8]);
	L2 = L*L;
    beta = 1/T;
	int *spin = new int[L2*Lt];
	int **nn = new int*[L2*Lt];
	for (i=0;i<L2*Lt;++i) nn[i] = new int[6];
    MakeStructure(L,Lt,nn);
    double **padd = new double*[L2];
    for (i=0;i<L2;++i) padd[i] = new double[6];
    
    
    // read coupling constants
    sprintf(confile,"sample%d/Jconfig",sampleidx);
    ifstream coupling;
    coupling.open(confile);
    for (i=0;i<L2;++i){
        for (j=0;j<4;++j) coupling >> padd[i][j];
    }
    coupling.close();
    sprintf(confile,"sample%d/Gconfig",sampleidx);
    coupling.open(confile);
    for (i=0;i<L2;++i){
        coupling >> padd[i][4];
        padd[i][5] = padd[i][4];

    }
    coupling.close();

    for (i=0;i<L2;++i){
        for (j=0;j<6;++j) padd[i][j] = 1-exp(-2*beta*padd[i][j]);
    }

    if (FIRST==1){
	    for (i=0;i<L2*Lt;++i) spin[i] = ( uni(gen) > 0.5 ? 1 : -1 );
    } else{
        sprintf(confile,"sample%d/L%dLt%d.conf",sampleidx,L,Lt);
        ifstream configuration;
        configuration.open(confile);
        for (i=0;i<L2*Lt;++i){
            configuration >> spin[i];
        }
        configuration.close();
    }
    //for (i=0;i<L2;++i) cout << spin[i] << "\n";
	int *stack = new int[L2*Lt];	

	sprintf(dSfile,"sample%d/L%dLt%dT%.4lfdS",sampleidx,L,Lt,T);
	ofstream datafile;
	datafile.open(dSfile,ios::app);
    datafile.precision(10);
	for (eq=0;eq<EQstep;++eq) WolffStep(L2,Lt,padd,spin,nn,stack);
    for (measure=0;measure<Nmeasure;++measure){
		m=m2=m4=0.0;
		for (count=0;count<Measurestep;++count){
			WolffStep(L2,Lt,padd,spin,nn,stack);
			mr = Magnetization(L2,Lt,spin);
			m += fabs(mr);
			m2 += mr*mr;
			m4 += mr*mr*mr*mr;
		}
		m /= Measurestep;
		m2 /= Measurestep;
		m4 /= Measurestep;
		datafile << T << " " << m << " " << m2 << " " << m4 << " " << L << " " << Lt << "\n";
	}
	datafile.close();

    //sprintf(confile,"sample%d/L%dLt%d.conf",sampleidx,L,Lt);
    //ofstream configuration;
    //configuration.open(confile);
    //for (i=0;i<L2*Lt;++i) configuration << spin[i] << " ";
    //configuration.close();
		
	delete[] spin;
	for (i=0;i<L2*Lt;++i) delete[] nn[i];
	delete[] nn;	
    for (i=0;i<L2*Lt;++i) delete[] padd[i];
    delete[] padd;
	delete[] stack;
}

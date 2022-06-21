BEGIN{
    L=16
    Lt=16
    T=2.0
    dT=0.1
    EQstep=30000
    Measurestep=5000
    Nmeasure=300

    #printf("#!/bin/bash\n")
    #printf("#PBS -l nodes=1:ppn=1\n")
    #printf("#PBS -q gall\n")
    #printf("#PBS -N randomL%d\n",L)
    #printf("cd $PBS_O_WORKDIR\n\n")

    printf("./wolff %d %d %.4f %d %d %d 1\n",L,Lt,T,EQstep,Measurestep,Nmeasure)
    for (i=0;i<40;++i){
        printf("./wolff %d %d %.4f %d %d %d 0\n",L,Lt,T,EQstep,Measurestep,Nmeasure)
        T += dT
    }
}

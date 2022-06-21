#!/bin/bash

for ((i=$1;i<$2;++i))
do
    mkdir sample$i
    ./makeJ $3 $i
done

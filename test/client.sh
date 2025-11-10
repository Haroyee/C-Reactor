#!/bin/bash
for ((i=0; i<4000; i+=1)); do
    ./test/client &
done
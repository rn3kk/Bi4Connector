#!/bin/bash

for i in {1..1000}
do
  echo "Write thread num $i" | nc 127.0.0.1 3333 -w 1&
done
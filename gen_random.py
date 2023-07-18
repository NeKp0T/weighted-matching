#!/opt/anaconda3/bin/python
from random import random
from sys import argv


n = int(argv[1])
gen_weights = (len(argv) > 2 and argv[2] == "-w")

print(n)

for i in range(1, n + 1):
    for j in range(1, i):
        if random() < 3 / (n - 1):
            if (not gen_weights):
                print(i, j)
            else:
                print(i, j, "%.3f" % random())

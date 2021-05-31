import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys

path_v0  = sys.argv[1]
path_v1  = sys.argv[2]
path_v2  = sys.argv[3]
title = sys.argv[4]

v0 =  pd.read_csv(path_v0, sep=',', engine='python',header=None)
v1 =  pd.read_csv(path_v1 , sep=',', engine='python',header=None)
v2 = pd.read_csv(path_v2, sep=',', engine='python',header=None)
if(len(sys.argv) == 6):
    base = pd.read_csv(sys.argv[5], sep=',', engine='python',header=None)
    plt.plot(base[1],base[0],linestyle="dashed",label="base")

plt.plot(v0[1],v0[0],linestyle="dashed",label="v0")
plt.plot(v1[1],v1[0],linestyle="dashed",label="v1")
plt.plot(v2[1],v2[0],linestyle="dashed",label="v2")

plt.grid(alpha=0.5, linestyle=':')
plt.legend()
plt.title(title)
plt.xlabel("N")
plt.ylabel("Cycles")
plt.savefig(title + ".svg", format="svg")
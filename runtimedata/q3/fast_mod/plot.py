import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys

base =  pd.read_csv("base.txt", sep=',', engine='python',header=None)
fast_mod1 =  pd.read_csv("fast_mod.txt", sep=',', engine='python',header=None)
fast_mod2 = pd.read_csv("fast_mod2.txt", sep=',', engine='python',header=None)

print(base[0])
print(fast_mod1)
print(fast_mod2)


fig, ax = plt.subplots(0,1)

# (base[0],base[1])
plt.plot(base[1],base[0],linestyle="dashed",label="base")
plt.plot(fast_mod1[1],fast_mod1[0],linestyle="dashed",label="fast1")
plt.plot(fast_mod2[1],fast_mod2[0],linestyle="dashed",label="fast2")
# base.plot(1,0,label="base",style='--',color="red")
# fast_mod1.plot(1,0,label="fast1",style='--',color="blue")
# fast_mod2.plot(1,0,label="fast2",style='--',color="green")
plt.grid(alpha=0.5, linestyle=':')
plt.legend()
plt.xlabel("N")
plt.ylabel("Cycles")
plt.savefig('fast_mod.svg', format="svg")
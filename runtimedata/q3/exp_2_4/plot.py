import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys


fig, ax = plt.subplots()

v0 = pd.read_csv("straight_2_4_rand", sep=',', engine='python',header=None)
v1 =  pd.read_csv("block_2_4_rand", sep=',', engine='python',header=None)
v2 =  pd.read_csv("fast_mod_2_4_rand" , sep=',', engine='python',header=None)
v3 =  pd.read_csv("fast_recon_2_4_rand" , sep=',', engine='python',header=None)



v0[0] = v0[0].apply(lambda x : x / 1000000000)
v1[0] = v1[0].apply(lambda x : x / 1000000000)
v2[0] = v2[0].apply(lambda x : x / 1000000000)
v3[0] = v3[0].apply(lambda x : x / 1000000000)
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)

# plt.plot(v0[1],v0[0],linestyle="dashed",label="v0",color="slategrey")
plt.plot(v1[1],v1[0],linestyle="dashed",label="v1",color="navajowhite")
plt.plot(v2[1],v2[0],linestyle="dashed",label="v2",color="plum")
plt.plot(v3[1],v3[0],linestyle="dashed",label="v2",color="darksalmon")
plt.grid(alpha=0.5, linestyle=':')
plt.xlabel("N")
ax.set_ylabel("Gigacycles")
plt.ticklabel_format(style='plain') 



plt.savefig("runtime.svg", format="svg")
plt.close()
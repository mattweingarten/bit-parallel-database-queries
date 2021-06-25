import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys
fig, ax = plt.subplots()

v0 = pd.read_csv("baseline", sep=',', engine='python',header=None)
v1 = pd.read_csv("l1_block", sep=',', engine='python',header=None)
v2 = pd.read_csv("fast_recon", sep=',', engine='python',header=None)
v3 = pd.read_csv("l2_block", sep=',', engine='python',header=None)
v4 = pd.read_csv("vector_v5", sep=',', engine='python',header=None)




print(v0[0][0])
for i in range(len(v0[0])):
    # v0[0][i] = v0[0][i] / (v0[1][i] * v0[1][i])
    v1[0][i] = v1[0][i] / (v1[1][i] * v1[1][i])
    v2[0][i] = v2[0][i] / (v2[1][i] * v2[1][i])
    v3[0][i] = v3[0][i] / (v3[1][i] * v3[1][i])
    v4[0][i] = v4[0][i] / (v4[1][i] * v4[1][i])
    
print(v0)


ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)



plt.plot(v1[1],v1[0],linestyle="dashed",color="plum",marker='+')
plt.plot(v2[1],v2[0],linestyle="dashed",color="darksalmon",marker='D')
plt.plot(v3[1],v3[0],linestyle="dashed",color="slategrey",marker='^')
plt.plot(v4[1],v4[0],linestyle="dashed",color="steelblue",marker='*')

plt.grid(alpha=0.5, linestyle=':')
plt.xlabel("Rows")
ax.set_ylabel("Cycle per pair")
plt.savefig("perf_q3.svg", format="svg")
plt.close()

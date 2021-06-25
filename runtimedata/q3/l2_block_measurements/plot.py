import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys



fig, ax = plt.subplots()


v10 =pd.read_csv("10K_l2block.txt", sep=',', engine='python',header=None)
v20 =pd.read_csv("20K_l2block.txt", sep=',', engine='python',header=None)
v40 =pd.read_csv("40K_l2block.txt", sep=',', engine='python',header=None)
v4 =pd.read_csv("4_l2block.txt", sep=',', engine='python',header=None)
v16 =pd.read_csv("16_l2block.txt", sep=',', engine='python',header=None)


for i in range(1,len(v10[0])):
    # print(i)
    v10[0][i] = v10[0][0] / v10[0][i]  
    v20[0][i] =  v20[0][0] / v20[0][i] 
    v40[0][i] = v40[0][0] /  v40[0][i] 
    v4[0][i] = v4[0][0] /  v4[0][i] 
    v16[0][i] = v16[0][0] /  v16[0][i] 

v10[0][0] = 1
v20[0][0] = 1
v40[0][0] = 1
v4[0][0] = 1
v16[0][0] = 1



ax.set_ylim([1,1.8])

plt.plot(v10[1],v10[0],linestyle="dashed",color="plum",marker='+')
plt.plot(v20[1],v20[0],linestyle="dashed",color="darksalmon",marker='D')
plt.plot(v40[1],v40[0],linestyle="dashed",color="slategrey",marker='^')
plt.plot(v4[1],v4[0],linestyle="dashed",color="steelblue",marker='*')
plt.plot(v16[1],v16[0],linestyle="dashed",color="chocolate",marker='h')



plt.grid(alpha=0.5, linestyle=':')
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
plt.xlabel("b")
ax.set_ylabel("Speedup")

ax.set_xticklabels(['2','2','8','32','128','512'])
plt.savefig("l2_block.svg", format="svg")
plt.close()

print(v10)
print(v20)
print(v40)
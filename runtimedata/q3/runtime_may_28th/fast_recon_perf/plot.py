import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys

fig, ax = plt.subplots()
v0 = pd.read_csv("v0",sep=",",engine="python")
v1 = pd.read_csv("v1",sep=",",engine="python")
v2 = pd.read_csv("v2",sep=",",engine="python")
cols = v0['cols']
tuples = (512 / cols).astype(int)
print(tuples)
ind = np.arange(5)
width = 0.2
rects1 = ax.bar(ind,v0['cycles'] / tuples , width, color='darksalmon')
rects1 = ax.bar(ind + width,v1['cycles'] / tuples , width, color='navajowhite')
rects1 = ax.bar(ind + 2 * width,v2['cycles'] / tuples , width, color='plum')


plt.hlines(32,ind[0] - width,ind[4] + 2 * width)
# print()

ax.set_title('Cost of reconstruction')
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)
plt.ticklabel_format(style='plain') 
ax.set_xticks(ind + width )
ax.set_xticklabels(['2', '4', '8', '16','32'])
ax.yaxis.grid(alpha=0.5, linestyle=':')
ax.set_ylabel("Cycles per tuple")
ax.set_xlabel("Cols")
plt.savefig("recon.svg", format="svg")
plt.close()
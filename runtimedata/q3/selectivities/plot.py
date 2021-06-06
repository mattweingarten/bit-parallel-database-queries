import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys


v0  =  pd.read_csv("selectiviy_fast_recon", sep=',', engine='python')
v1 =  pd.read_csv("selectiviy_vector5", sep=',', engine='python')

print(v0.columns.tolist())
fig, ax = plt.subplots()

v0['cycles'] = v0['cycles'].apply(lambda x : x / 1000000000)
v1['cycles'] = v1['cycles'].apply(lambda x : x / 1000000000)




plt.plot(v0['sc'],v0['cycles'],linestyle="dashed",color='darksalmon',marker='^')
plt.plot(v1['sc'],v1['cycles'],linestyle="dashed",color="plum",marker='D')
# v0.plot('cycles','select',
    

# )
# v1.plot('cycles','select',


# )
# print(v1)
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)
ax.grid(alpha=0.5, linestyle=':')
plt.ticklabel_format(style='plain') 
ax.set_ylabel("Giga cycles")
ax.set_xlabel("Selectivity")
plt.savefig("select.svg",format="svg")
# plt.savefig()
plt.close()
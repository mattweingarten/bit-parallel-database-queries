import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys


fig, ax = plt.subplots()
df = pd.read_csv("for_plot.txt", sep='\s+', engine='python')
# df.set_index('32')




x = np.linspace(0, len(df['rows']) - 1 , len(df['rows']))
# df.plot()
# print(len(x))
# print(len(df['rows']))
# print(len(df['parallel']))
# plt.plot(x,df['parallel'])
plt.plot(x,df['parallel'],linestyle="dashed",color="slategrey",marker="^",label="parallel")
plt.plot(x,df['vector'],linestyle="dashed",color="navajowhite",marker="*",label="vector")
plt.plot(x,df['unroll4x'],linestyle="dashed",color="plum",marker="D",label="unroll4x")
plt.plot(x,df['0check'],linestyle="dashed",color="darksalmon",marker="8",label="zero check")
plt.plot(x,df['earlypruneand0check'],linestyle="dashed",color="steelblue",marker="h",label="zero check & early prune")


plt.vlines(9,ymin=0.1, ymax=3.9,linestyles ='dotted')
plt.ticklabel_format(style='plain') 


# plt.title('Q1 performance\nX times slower than memory bandiwdth')
plt.title('Q1 performance')
plt.ylabel("Times slower than memory bandwidth")
plt.xlabel("Rows")
ax.set_ylim([0.5,4])
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
# ax.set_xticklabels(df['rows'],x)
ax.set_xticklabels(['128','128','512','2K','8K','32K','131K','524K','2097K'])
plt.grid(alpha=0.5, linestyle=':')
# plt.legend()
plt.savefig("q1_perf.svg", format="svg")
plt.close()
print(df)
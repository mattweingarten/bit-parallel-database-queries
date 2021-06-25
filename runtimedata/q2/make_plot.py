import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys


fig, ax = plt.subplots()
df = pd.read_csv("query2.csv", sep=';', engine='python')
# df.set_index('32')



x = np.linspace(0, len(df['rows']) - 1 , len(df['rows']))
plt.yscale('log',base=2) 
# df.plot()
# print(len(x))
# print(len(df['rows']))
# print(len(df['parallel']))
# plt.plot(x,df['parallel'])
plt.plot(x,df['simple-index'],linestyle="dashed",color="navajowhite",marker="*",label="simplified indexing")
plt.plot(x,df['scalar'],linestyle="dashed",color="plum",marker="+",label="scalar replacement")
plt.plot(x,df['scalar/vec'],linestyle="dashed",color="darksalmon",marker="D",label="scalar & vector accumulation ")
plt.plot(x,df['unroll'],linestyle="dashed",color="peru",marker="2",label="unroll 4x")
plt.plot(x,df['unroll/vec'],linestyle="dashed",color="olive",marker="h",label="unroll 4x & vector accumulation")
plt.plot(x,df['vectorized'],linestyle="dashed",color="slategrey",marker="v",label="Fully vectorized")


plt.vlines(9,ymin=0.1, ymax=130,linestyles ='dotted')
#plt.ticklabel_format(style='plain') 


# plt.title('Q1 performance\nX times slower than memory bandiwdth')
plt.title('Q2 performance')
plt.ylabel("Times slower than memory bandwidth (log scale)")
plt.xlabel("Rows")
ax.set_ylim([0.5,130])
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
# ax.set_xticklabels(df['rows'],x)
ax.set_xticklabels(['128','128','512','2K','8K','32K','131K','524K','2097K'])
plt.grid(alpha=0.5, linestyle=':')
# plt.legend()
plt.savefig("q2_perf.svg", format="svg")
plt.close()
print(df)
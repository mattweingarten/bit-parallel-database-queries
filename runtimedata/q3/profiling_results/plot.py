import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import matplotlib.ticker as ticker
import sys



fig, ax = plt.subplots()
data1 = [665332756 - 158679344,545093404-45117498,522651146-30108058]
data2 = [158679344,45117498,30108058]
df=pd.DataFrame({'data1':data1, 'data2':data2})
df.plot(kind='bar', stacked=True)
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)

plt.xticks()
plt.grid(alpha=0.5, linestyle=':', axis='y')
plt.savefig("cost_of_recon.svg", format="svg")
plt.close()

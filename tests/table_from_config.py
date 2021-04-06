import pandas as pd
import numpy as np
import sys
import random

np.set_printoptions(linewidth=200,suppress=True)

#default params
data_types = np.array(["FLOAT","INT","INDEX"])

DEFAULT_MAX = 10000
DEFAULT_MIN = 0

#open config file
filename = sys.argv[1]
config_file = "./data/config/" + filename 
table_name = "./data/tables/" + filename.split('.', 1)[0] + ".txt"
print(table_name)

config = np.genfromtxt(config_file,delimiter=",",skip_header=0,dtype=str)



# generate random with probability p smaller than x
def rand_gen(x,p):
    r = random.uniform(0, 1)
    if(r < p):
        return random.uniform(DEFAULT_MIN,x)
        
    else:
        return random.uniform(x, DEFAULT_MAX)

def populate_column(rng,column):
    for i in range(len(column)):
        column[i] = rand_gen(rng[0],rng[1])
    return column

def populate_column_index(column):
    for i in range(len(column)):
        column[i] = i


def create_table(table_stats):
    rows = int(table_stats[0])
    column_types = np.array([])
    ranges = []
    rng = np.array([])
    for column in table_stats[1:]:
        if np.isin(column,data_types):

            column_types = np.append(column_types,column)
            if(rng.size == 0):
                rng = np.append(rng,[DEFAULT_MAX,1])
            ranges.append(rng)
            rng = np.array([])
        else:
            rng = np.append(rng,float(column))
    ranges.pop(0)
    if(rng.size == 0):
        rng = np.append(rng,[DEFAULT_MAX,1])
    ranges.append(rng)
    
    assert(len(column_types) == len(ranges))
    columns = len(column_types)
    table = np.zeros((rows,columns))

    for i in range(columns):
        
        if(column_types[i] == "INDEX"):
            populate_column_index(table[:,i])
        elif(column_types[i] == "FLOAT"):
            populate_column(ranges[i],table[:,i])
        elif(column_types[i] == "INT"):
            populate_column(ranges[i],table[:,i])
            table[:,i] = table[:,i].astype(int)


    df = pd.DataFrame(table)
    for i in range(len(column_types)):
        if(column_types[i] == "INT" or column_types[i] == "INDEX"):
            df.iloc[:,i]  = df.iloc[:,i].astype(int)
    df.to_csv(table_name,header=False,index=False)
    return 


create_table(config)







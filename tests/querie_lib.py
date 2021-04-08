import numpy as np

results_path = "./data/groundtruth/"

table_path = "./data/tables/"
config_path = "./data/config/"


data_types = np.array(["FLOAT","INT","INDEX"])


def get_col_types(table_stats):
    
    table_stats = np.genfromtxt(table_stats,delimiter=",",skip_header=0,dtype=str)
    rows = int(table_stats[0])
    column_types = np.array([])
    ranges = []
    rng = np.array([])
    for column in table_stats[1:]:
        if np.isin(column,data_types):
            column_types = np.append(column_types,column)
            
    columns = len(column_types)
    table = np.zeros((rows,columns))

    fmt=""
    for i in range(columns):
        
        if(column_types[i] == "INDEX"):
            fmt += " %i,"
            
        elif(column_types[i] == "FLOAT"):
            fmt += " %10.32f,"
            
        elif(column_types[i] == "INT"):
            fmt += " %i,"
            
    if(fmt[-1]==','):
        fmt = fmt[:-1]

    
    return (column_types,fmt) 




def get_table(tableName,path):
    s = path + tableName + ".txt"

    array = np.genfromtxt(s,delimiter=',')
    
    return array

def q1(tablename,col, val):
    # q1
    # Select * from t1 where A > val
    
    
    
    result = []
    T = get_table(tablename,table_path)

    col_types,fmt = get_col_types(config_path + tablename + ".config")
    
    
    
    if(col_types[0]!="INDEX"):
        c = ord(col) - 65 # converting col into int
        
    else: 
        if(col == "ID"):
            c = 0
        else:
            c = ord(col) - 65 # converting col into int
            c += 1 
    
    #### Actual Query

    for i in range(len(T)):

        if(T[i,c] > val):
            result.append(T[i,:])
    
    result = np.vstack(result)

    ### saving
    
    np.savetxt(results_path + "q1_" + tablename + ".csv",result,fmt=fmt)
    

    return

def q2(tablename,col,val):
    

    # q2 
    # Select Sum(D) from t1 where A > val

    result = 0.
    T = get_table(tablename,table_path)

    col_types,fmt = get_col_types(config_path +tablename+".config")
    
    
    
    if(col_types[0]!="INDEX"):
        c = ord(col) - 65 # converting col into int
        
    else: 
        if(col == "ID"):
            c = 0
        else:
            c = ord(col) - 65 # converting col into int
            c += 1 
    
    #### Actual Query

    for i in range(len(T)):
        if(T[i,1] > val):
            result += T[i,4]
    
    ### saving
    
    np.savetxt(results_path + "q2_" + tablename + ".csv",np.array([result]),fmt="%10.32f") # should be only one value

    return
    
def q3(tablename1,col1,tablename2,col2,col3):    
    # q3
    # Select * From R,S Where S.A % R.B = R.C
    
    
    result = []
    R = get_table(tablename2,table_path)
    S = get_table(tablename1,table_path)
    
    col_types1, fmt1 = get_col_types(config_path +tablename1+".config")
    col_types2,fmt2 = get_col_types(config_path +tablename2+".config")
    
    if(col_types1[0]!="INDEX"):
        c1 = ord(col1) - 65 # converting col into int
        
    else: 
        if(col1 == "ID"):
            c1 = 0
        else:
            c1 = ord(col1) - 65 # converting col into int
            c1 += 1 
    
    if(col_types2[0]!="INDEX"):
        c2 = ord(col2) - 65 # converting col into int
        c3 = ord(col3) - 65
         
        
    else: 
        if(col2 == "ID"):
            c2= 0
        else:
            c2 = ord(col2) - 65 # converting col into int
            c3 = ord(col3) - 65
            c2 += 1 
            c3 += 1
    
    
        
    
    for i in range(len(S)):
        for j in range(len(R)):
            if(R[j,c2]!=0):
            
                if(S[i,c1] % R[j,c2] == R[j,c3]):
                    result.append(np.concatenate((R[j,:],S[i,:])))

    result = np.vstack(result)
    
    ### saving
    
    np.savetxt(results_path + "q3_" + tablename1 + "_" + tablename2 + ".csv",result,fmt= fmt1+","+fmt2)
    
    return

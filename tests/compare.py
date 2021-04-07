results_path = "./groundtruth/"
table_path = "./data/tables/"
import sys

tablename = table_path + sys.argv[1]
resultname= results_path + sys.argv[2]

with open(tablename, 'r') as tablefile, open(resultname, 'r') as resultsfile:
    table = tablefile.readlines()
    results = resultsfile.readlines()

with open('comparison.csv', 'w') as outFile:
    for line in results:
        if line not in table:
        	print("This data point was incorrectly excluded: \n" + line)

with open('comparison.csv', 'w') as outFile:
    for line in table:
        if line not in results:
        	print("This data point was incorrectly included: \n" + line)

import json
import csv
import datetime
import sys

infile=sys.argv[1]
filename = '%s.csv'%datetime.datetime.now().strftime("%Y%m%d")
with open(filename, 'w') as csvfile:
	fieldnames = ['OccurredAt', 'bank', 'name', 'HKD', 'RMB', 'USD']
	writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
	writer.writeheader()
	 
	for line in open(infile,'r').readlines():
		line = line.strip()
		if line.endswith('---'):
			line =line[:-3]
			
		line = line.replace('=>',':')
		if len(line)==0:
			continue
		#print (line)
		t = (json.loads(line.replace('=>',':')))
		print (t)
		print (t["Value 1"]["bank"])
		t1=t["Value 1"]
		for t11 in t1['balance']:
			writer.writerow({'OccurredAt': t["OccurredAt"], 'bank': t1['bank'], 'name':t11['name'], t11['type']:t11['balance']})
   
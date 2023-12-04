import random

r = []
for i in range(10000):
    r.append(int(random.random() * 999999))

with open('very_large_file.txt','w') as f:
    f.write('\n'.join([str(x) for x in r])+'\n')

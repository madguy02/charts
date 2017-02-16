f = open("line.html",'r')
filedata = f.read()
f.close()

newdata = filedata.replace("sale: 100","sale: 120")


f = open(fileout,'w')
f.write(newdata)
f.close()
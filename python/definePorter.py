


def getFromFile (file):
    f = open (file, "r")
    ls = []
    content = f.readlines()
    [x.strip() for x in content] 
    f.close()
    return content


f = open("mcp2515_defs.py", "w")
for line in getFromFile("mcp2515_defs.h"):
    
    splited = line.split()
    if  len(splited) != 3:
        pass
    elif (splited[0] == "#define"): 
        if len(splited[2]) > 1 :
            if splited[2][1] =="x":
                writeToFile =splited[1]+" = b'" +"\\"+splited[2][1:] + "'"
            else:
                writeToFile =splited[1]+" = "+splited[2]
        else:
            writeToFile =splited[1]+" = "+splited[2]
        print(writeToFile)
        f.write(writeToFile + '\n')
    
f.close()
    
        

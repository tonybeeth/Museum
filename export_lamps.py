import bpy
import math

f = open("modular.dat", "w")

objBaseNames = ["brick_wall", "brick_door", "lamp_holder"]#,"bulb", "Cone"]
f.write(str(len(objBaseNames))+'\n')
#primary location of objects assumed to be the origin(0,0,0)

#print info for brick walls
for baseName in objBaseNames:
    f.write(baseName+'\n')
    for obj in bpy.data.objects:
        if baseName in obj.name:
            loc = obj.location
            rot = obj.rotation_euler
            
            f.write(obj.name+'\n')
            f.write(str(loc[0])+" "+str(loc[1])+" "+str(loc[2])+'\n')
            f.write(str(rot[0]*180/math.pi)+" "+str(rot[1]*180/math.pi)+" "+str(rot[2]*180/math.pi)+'\n')
    f.write('\n')
        
f.close()
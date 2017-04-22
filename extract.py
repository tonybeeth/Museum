import bpy
import math

f = open("modular.dat", "w")

#primary location of objects assumed to be the origin(0,0,0)

#print info for brick walls
f.write("Walls\n")
for obj in bpy.data.objects:
    if "brick_wall" in obj.name:
        loc2 = obj.location
        rot2 = obj.rotation_euler
        
        f.write(str(obj.name))
        f.write(" ")
        f.write(str(loc2[0]))
        f.write(" ")
        f.write(str(loc2[2]))
        f.write(" ")
        f.write(str(-loc2[1]))
        f.write('\n')
        
        f.write(str(rot2[0]*180/math.pi))
        f.write(" ")
        f.write(str(rot2[2]*180/math.pi))
        f.write(" ")
        f.write(str(rot2[1]*180/math.pi))
        f.write("\n")


#print info for wall joints
f.write("\nDoors\n")
for obj in bpy.data.objects:
    if "brick_door" in obj.name:
        loc2 = obj.location
        rot2 = obj.rotation_euler
        
        f.write(str(obj.name))
        f.write(" ")
        f.write(str(loc2[0]))
        f.write(" ")
        f.write(str(loc2[2]))
        f.write(" ")
        f.write(str(-loc2[1]))
        f.write('\n')
        
        f.write(str(rot2[0]*180/math.pi))
        f.write(" ")
        f.write(str(rot2[2]*180/math.pi))
        f.write(" ")
        f.write(str(rot2[1]*180/math.pi))
        f.write("\n")
        
f.close()
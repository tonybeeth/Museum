import bpy
import math

f = open("out.dat", "w")
origin_wall = bpy.data.objects
    
sw = bpy.context.scene.objects.active
#sj = bpy.data.objects["attach.006"]

bpy.ops.object.duplicate()

nw = bpy.context.scene.objects.active
#for obj in bpy.data.objects:
#    if "wall" in obj.name:
#        nw = obj
        
#nw.location = (sw.location[0], sw.location[1]+.2+sw.dimensions[1], sw.location[2])
nw.location = (sw.location[0]+.9+.2, sw.location[1], sw.location[2])

f.close()
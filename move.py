import bpy
import math

for obj in bpy.data.objects:
    if "wall" in obj.name or "attach" in obj.name:
        obj.select = True
        #bpy.ops.object.origin_set(type='ORIGIN_GEOMETRY')
     #   obj.select
        #obj.location[2] = bpy.data.objects['concrete_floor'].location[2]+.45+bpy.data.objects['concrete_floor'].dimensions[1]/2


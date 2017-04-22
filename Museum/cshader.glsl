#version 430 compatibility
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout( std140, binding=4) buffer Pos
{
	vec4 Positions[ ];    /*You can use the empty brackets, but only on the
							last element of the buffer. The actual dimension
							will be determined for you when OpenGL examines
							the size of this buffer's data store.*/
};

layout( std140, binding=5) buffer Vel
{
	vec4 Velocities[ ];
};

layout( std140, binding=6) buffer Col
{
	vec4 Colors[ ];
};

layout( local_size_x =128, local_size_y = 1, local_size_z = 1) in;

const vec4 SPHERE = vec4(0., 0., 0., 1); //x,y,z,r of our sphere
											//could also have this passed in
vec3 Bounce(vec3 vin, vec3 n)
{
	vec3 vout = reflect(vin, n);
	return vout;
}

vec3 BounceSphere(vec3 p, vec3 v, vec4 s)
{
	vec3 n = normalize(p - s.xyz);
	return Bounce(v, n);
}

bool IsInsideSphere(vec3 p, vec4 s)
{
	//get length between p and center of sphere
	float r = length(p-s.xyz);
	//check if length is less than radius of sphere
	return (r<s.w);
}

void main()
{
	const vec3 G = vec3(0., -9.8, 0.);
	const float DT = 0.005;

	uint gid = gl_GlobalInvocationID.x;		//the y and z values are both 1 in this case

	//get position and velocity
	vec3 p = Positions[gid].xyz;
	vec3 v = Velocities[gid].xyz;

	//compute new position and new velocity using equations of motion
	vec3 pp = p + v*DT + .5*DT*DT*G;
	vec3 vp = v + G*DT;

	if(IsInsideSphere(pp, SPHERE)){
		//Make bounce happen from previous position in space
		//Bouncing from surface is time consuming
		//if DT is small enough, NO ONE WILL EVER KNOW!!!
		vp = BounceSphere(p,v,SPHERE);
		pp = p+vp*DT + .5*DT*DT*G;
	}

	//update velocity and position
	Positions[gid].xyz = pp;
	Velocities[gid].xyz = vp;
}
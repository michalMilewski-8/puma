#version 330 core

uniform mat4 vieww;
uniform mat4 proj;

layout (location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;
out float near; //0.01
out float far; //100

// Grid position are in clipped space
vec3 gridPlane[6] = vec3[] (
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    near = 0.01;
    far = 100;
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, vieww, proj).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, vieww, proj).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}
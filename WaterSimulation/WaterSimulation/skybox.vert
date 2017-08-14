#version 440

layout (location = 0) in vec3 vertexPosition;

out vec4 Position;
out vec3 ReflectDir;

uniform mat4 mvp;
uniform mat4 ModelViewMatrix;

void main(void)
{
   vec4 coord3d = vec4(vertexPosition, 1.0f);
   Position = ModelViewMatrix * coord3d;
   ReflectDir = vertexPosition;

   gl_Position = mvp * coord3d;
}

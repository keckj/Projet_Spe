
#version 330
in vec2 vertexPos;
in vec2 texCoord;

layout(std140) uniform projectionView {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec3 cameraPos;
	vec3 cameraDir;
	vec3 cameraUp;
	vec3 cameraRight;
};

out VS_FS_VERTEX {
	vec2 texCoord;
} vertex_out;

void main (void)
{	
	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPos,0,1);
	vertex_out.texCoord = texCoord;
}

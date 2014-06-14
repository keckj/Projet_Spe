
#version 330

in VS_FS_VERTEX {
	vec2 texCoord;
} vertex_in;

struct Colormap {
	vec4[10] colors;
};

layout(std140) uniform colorMaps {
	Colormap colormap[12];
} in_colormaps;

uniform sampler2D matrix;
uniform float minVal = 0.0f;
uniform float maxVal = 1.0f;
uniform int colormapId = 0;

out vec4 out_colour;

void main (void)
{	
	float nColors = 9.0f;
	float val = texture(matrix, vertex_in.texCoord).r;
	float t = (val-minVal)/(maxVal-minVal);
	int i = int(t*nColors);
	float dt = 1.0f/nColors;
	float tl = (t-float(i)*dt)/dt;
	
	Colormap colormap = in_colormaps.colormap[colormapId];
	out_colour = mix(colormap.colors[i], colormap.colors[i+1], tl)/255;
}


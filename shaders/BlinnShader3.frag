#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 DlightDir;		// direction of the direct light
	vec3 DlightColor;	// color of the direct light
	vec3 AmbLightColor;	// ambient light
	vec3 eyePos;		// position of the viewer
	vec3 lightPos;	// position of the spotlight
	vec3 lightDir;	// direction of the spotlight
	vec4 lightColor;// color of the spotlight	
} gubo;


layout(set = 1, binding = 0) uniform UniformBufferObject {
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

const float beta = 2.0f;
const float g = 1.5;
const float cosout = 0.85;
const float cosin  = 0.95;
const float gamma = 160.0f;	// cosine power for the Blinn specular reflection

layout(set = 1, binding = 1) uniform sampler2D tex;

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);

	vec3 lightDir = vec3(normalize(gubo.lightPos - fragPos)); //vec3(0.0,1.0,0.0);
	vec3 lightColor = vec3(gubo.lightColor) * vec3(pow( (g / length(gubo.lightPos - fragPos)), beta)) * clamp( (dot(lightDir, vec3(gubo.lightDir)) - cosout) / (cosin - cosout) , 0, 1); //vec3(1.0,0.15,0.2);
	float dist = length(gubo.lightPos - fragPos);

	vec3 Diffuse = texture(tex, fragUV).rgb * clamp(dot(Norm, lightDir), 0.0, 1.0)* 0.995f ;
	vec3 Specular = vec3(pow(clamp(dot(Norm, normalize(lightDir + EyeDir)), 0.0, 1.0), 160.0f));
	vec3 Ambient = texture(tex, fragUV).rgb * 0.005;
	
	outColor = vec4(clamp((Diffuse + Specular) * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}
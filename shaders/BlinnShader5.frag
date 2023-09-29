#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightPos1;
	vec3 lightDir1;
	vec4 lightColor1;
	vec3 lightPos2;
	vec3 lightDir2;
	vec4 lightColor2;
	vec3 lightPos3;
	vec3 lightDir3;
	vec4 lightColor3;
	vec3 eyePos;
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
const float cosin  = 0.90;
const float gamma = 160.0f;	// cosine power for the Blinn specular reflection

layout(set = 1, binding = 1) uniform sampler2D tex;

void main() {

	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);

	vec3 lightDir1 = vec3(normalize(gubo.lightPos1 - fragPos)); //vec3(0.0,1.0,0.0);
	vec3 lightColor1 = vec3(gubo.lightColor1) * vec3(pow((g / length(gubo.lightPos1 - fragPos)), beta)) * clamp((dot(lightDir1, vec3(gubo.lightDir1)) - cosout) / (cosin - cosout), 0, 1); //vec3(1.0,0.15,0.2);
	vec3 Diffuse1 = texture(tex, fragUV).rgb * clamp(dot(Norm, lightDir1), 0.0, 1.0) * 0.995f;
	vec3 Specular1 = MS * vec3(pow(clamp(dot(Norm, normalize(lightDir1 + EyeDir)), 0.0, 1.0), 160.0f));
	vec3 Ambient1 = texture(tex, fragUV).rgb * 0.005f;
	

	vec3 lightDir2 = vec3(normalize(gubo.lightPos2 - fragPos)); //vec3(0.0,1.0,0.0);
	vec3 lightColor2 = vec3(gubo.lightColor2) * vec3(pow((g / length(gubo.lightPos2 - fragPos)), beta)) * clamp((dot(lightDir2, vec3(gubo.lightDir2)) - cosout) / (cosin - cosout), 0, 1); //vec3(1.0,0.15,0.2);
	vec3 Diffuse2 = texture(tex, fragUV).rgb * clamp(dot(Norm, lightDir2), 0.0, 1.0) * 0.995f;
	vec3 Specular2 = MS * vec3(pow(clamp(dot(Norm, normalize(lightDir2 + EyeDir)), 0.0, 1.0), 160.0f));
	vec3 Ambient2 = texture(tex, fragUV).rgb * 0.005f;	


	vec3 lightDir3 = vec3(normalize(gubo.lightPos3 - fragPos)); //vec3(0.0,1.0,0.0);
	vec3 lightColor3 = vec3(gubo.lightColor3) * vec3(pow((g / length(gubo.lightPos3 - fragPos)), beta)) * clamp((dot(lightDir3, vec3(gubo.lightDir3)) - cosout) / (cosin - cosout), 0, 1); //vec3(1.0,0.15,0.2);
	vec3 Diffuse3 = texture(tex, fragUV).rgb * clamp(dot(Norm, lightDir3), 0.0, 1.0) * 0.995f;
	vec3 Specular3 = MS * vec3(pow(clamp(dot(Norm, normalize(lightDir3 + EyeDir)), 0.0, 1.0), 160.0f));
	vec3 Ambient3 = texture(tex, fragUV).rgb * 0.005f;

	outColor = vec4(clamp((Diffuse1 + Specular1) * lightColor1.rgb + Ambient1,0.0,1.0), 1.0f) + vec4(clamp((Diffuse2 + Specular2) * lightColor2.rgb + Ambient2,0.0,1.0), 1.0f) + vec4(clamp((Diffuse3 + Specular3) * lightColor3.rgb + Ambient3,0.0,1.0), 1.0f);

}
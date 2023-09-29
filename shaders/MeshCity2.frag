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

const float beta = 1.0f;	// decay exponent of the spotlight
const float g = 5;			// target distance of the spotlight
const float cosout = 0.5;	// cosine of the outer angle of the spotlight
const float cosin  = 0.95;	// cosine of the inner angle of the spotlight

const float gamma = 160.0f;	// cosine power for the Blinn specular reflection

// coefficients for the spehrical harmonics ambient light term
const vec3 C00  = vec3( .38f, .43f, .45f)/8.0f;
const vec3 C1m1 = vec3( .29f, .36f, .41f)/8.0f;
const vec3 C10  = vec3( .04f, .03f, .01f)/8.0f;
const vec3 C11  = vec3(-.10f,-.10f,-.09f)/8.0f;
const vec3 C2m2 = vec3(-.06f,-.06f,-.04f)/8.0f;
const vec3 C2m1 = vec3( .01f,-.01f,-.05f)/8.0f;
const vec3 C20  = vec3(-.09f,-.13f,-.15f)/8.0f;
const vec3 C21  = vec3(-.06f,-.05f,-.04f)/8.0f;
const vec3 C22  = vec3( .02f, .00f,-.05f)/8.0f;

layout(set = 1, binding = 1) uniform sampler2D tex;
void main() {

	vec3 albedo = texture(tex, fragUV).rgb;		// main color
	vec3 MD = albedo;
	vec3 MS = ubo.sColor;
	vec3 MA = albedo * ubo.amb;
	vec3 LA = gubo.AmbLightColor;	

	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	
	// replace the following lines with the code to implement a spot light model (based on point lights)
	// with the light color in gubo.lightColor, the position in gubo.lightPos,
	// and the direction in gubo.lightDir.
	// The exponent of the decay is in constant variable beta, and the base distance
	// is in constant g, the cosines of the inner and outer angle are respectively
	// in constants cosin and cosout

	
	vec3 lightColor = vec3(1.0,1.0,1.0);
	vec3 lightDir = normalize(gubo.lightPos - fragPos);
	float dist = length(gubo.lightPos - fragPos);
	float attenuation = pow((g/dist), beta); 

	vec3 Diffuse = texture(tex, fragUV).rgb * attenuation * clamp(dot(Norm, normalize(gubo.lightPos-fragPos)), 0.0, 1.0);
	vec3 Specular = MS*vec3(pow(clamp(dot(Norm, normalize(lightDir + EyeDir)),0.0,1.0), 160.0f)) * attenuation;
	vec3 Ambient = texture(tex, fragUV).rgb * 0.01f * attenuation;
	
	outColor = vec4(clamp((Diffuse + Specular) * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}
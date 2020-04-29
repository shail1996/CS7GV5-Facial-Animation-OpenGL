#version 330 core

#define PI 3.14159265

in vec3 vsNormals;
in vec3 vsViewDir;
in vec3 vsLightDir;
in float vsLightDistance;

uniform vec3 diffuseColor = vec3(0, 0, 0.5);
uniform vec3 specularColor = vec3(1, 1, 1);
uniform float F0 = 1.8;
uniform float roughness = 0.1;
uniform float k = 0.6;
uniform vec3 lightColor = vec3(1, 1, 1);

out vec3 frag_colour;

vec3 CookTorrance(vec3 materialDiffuseColor,
	vec3 materialSpecularColor,
	vec3 normal,
	vec3 lightDir,
	vec3 viewDir,
	vec3 lightColor)
{
	float NdotL = max(0, dot(normal, lightDir));
	float Rs = 0.0;
	if (NdotL > 0) 
	{
		vec3 H = normalize(lightDir + viewDir);
		float NdotH = max(0, dot(normal, H));
		float NdotV = max(0, dot(normal, viewDir));
		float VdotH = max(0, dot(lightDir, H));

		// Fresnel reflectance
		float F = pow(1.0 - VdotH, 5.0);
		F *= (1.0 - F0);
		F += F0;

		// Microfacet distribution by Beckmann
		float m_squared = roughness * roughness;
		float r1 = 1.0 / (4.0 * m_squared * pow(NdotH, 4.0));
		float r2 = (NdotH * NdotH - 1.0) / (m_squared * NdotH * NdotH);
		float D = r1 * exp(r2);

		// Geometric shadowing
		float two_NdotH = 2.0 * NdotH;
		float g1 = (two_NdotH * NdotV) / VdotH;
		float g2 = (two_NdotH * NdotL) / VdotH;
		float G = min(1.0, min(g1, g2));

		Rs = (F * D * G) / (PI * NdotL * NdotV);
	}
	return materialDiffuseColor * lightColor * NdotL + lightColor * materialSpecularColor * NdotL * (k + Rs * (1.0 - k));
}

void main()
{
	frag_colour = CookTorrance(diffuseColor,
		specularColor,
		vsNormals,
		vsLightDir,
		vsViewDir,
		lightColor);
}
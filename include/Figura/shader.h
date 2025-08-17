#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <iostream> 
#include <fstream>

namespace fgr {

	// DEFAULT SHADERS FOR RENDERING MODELS 

static const char* default_vs = R"(
#version 460 core

layout (location = 0) in vec3 _vpos;
layout (location = 2) in vec2 _vtexcoords;
layout (location = 3) in vec3 _vnormal;
layout (location = 4) in vec3 _vtangent;
layout (location = 5) in vec3 _vbittangent;

out vec2 _ftexcoords;
out vec3 _fnormal;
out vec3 _ftangent;
out vec3 _fbittangent;
out vec3 _fragPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(_vpos,1.0);
	_ftexcoords = _vtexcoords;
	_fnormal = mat3(normalMatrix) * _vnormal;
	_ftangent = mat3(normalMatrix) * _vtangent;
	_fbittangent = mat3(normalMatrix) * _vbittangent;
	_fragPos = vec3(modelMatrix * vec4(_vpos,1.0));
}
)";


	static const char* pbrShader = R"(
#version 460 core

// -----------------------------------------------------------------------------
// INCOMING DATA FROM VERTEX SHADER
// -----------------------------------------------------------------------------
in vec2 _ftexcoords;
in vec3 _fnormal;
in vec3 _ftangent;
in vec3 _fbittangent;
in vec3 _fragPos;

// -----------------------------------------------------------------------------
// UNIFORMS & TEXTURES
// -----------------------------------------------------------------------------
uniform vec3 viewPos;

// Directional Light:
struct D_LIGHT {
    float x,y,z;
    float r,g,b;
};

layout (std430, binding = 0) buffer d_lights {
    int d_count;
    D_LIGHT d_light[];
};

// Point Light:
struct P_LIGHT {
    float x,y,z;
    float r,g,b;
};

layout (std430, binding = 1) buffer p_lights {
    int p_count;
    P_LIGHT p_light[];
};

// Spot Light:
struct S_LIGHT {
    float x,y,z;
    float r,g,b;
    float dx,dy,dz;
    float angle;
};

layout (std430, binding = 2) buffer s_lights {
    int s_count;
    S_LIGHT s_light[];
};

// PBR Maps:
layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalmap;
layout (binding = 2) uniform sampler2D metallicMap;
layout (binding = 3) uniform sampler2D roughnessMap;
layout (binding = 4) uniform sampler2D aoMap; // Ambient Occlusion

// -----------------------------------------------------------------------------
// FRAGMENT SHADER OUTPUT
// -----------------------------------------------------------------------------
out vec4 final_color;

// -----------------------------------------------------------------------------
// PBR HELPER FUNCTIONS 
// -----------------------------------------------------------------------------

// Normal Distribution Function (GGX Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265359 * denom * denom;
    
    return num / denom;
}

// Geometry Function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel Function (Schlick approximation)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// -----------------------------------------------------------------------------
// MAIN FRAGMENT SHADER FUNCTION
// -----------------------------------------------------------------------------
void main() {
    // 1. Get material properties from textures.
    vec3 albedo = pow(texture(albedoMap, _ftexcoords).rgb, vec3(2.2)); // Gamma correction
    float metallic = texture(metallicMap, _ftexcoords).r;
    float roughness = texture(roughnessMap, _ftexcoords).r;
    float ao = texture(aoMap, _ftexcoords).r;

    // 2. Calculating the normal: use normal map if available.
    vec3 normVector = normalize(_fnormal);
    mat3 TBN = mat3(normalize(_ftangent), normalize(_fbittangent), normalize(_fnormal));

    if (texture(normalmap, _ftexcoords).rgb != vec3(0.5, 0.5, 1.0)) {
        normVector = TBN * (texture(normalmap, _ftexcoords).rgb * 2.0 - 1.0);
    }
    
    // Normalize the final normal vector.
    normVector = normalize(normVector);

    // 3. Prepare the necessary vectors for lighting calculations.
    vec3 N = normVector;
    vec3 V = normalize(viewPos - _fragPos);
    
    // 4. Define the base reflectivity (F0) based on material type.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // 5. Accumulate the total outgoing light (Lo) from all light sources.
    vec3 result = vec3(0.0);

    // -------------------------------------------------------------------------
    // Directional Light Calculations
    // -------------------------------------------------------------------------
    for(int a = 0; a < d_count; a++) {
        vec3 L = normalize(-vec3(d_light[a].x, d_light[a].y, d_light[a].z));
        vec3 H = normalize(V + L);
        
        // PBR BRDF terms
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        // Diffuse and specular contributions
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        // Final color contribution
        vec3 radiance = vec3(d_light[a].r, d_light[a].g, d_light[a].b);
        float NdotL = max(dot(N, L), 0.0);
        result += (kD * albedo / 3.14159265359 + specular) * radiance * NdotL;
    }       

    // -------------------------------------------------------------------------
    // Point Light Calculations
    // -------------------------------------------------------------------------
    for(int a = 0; a < p_count; a++) {
        vec3 L = normalize(vec3(p_light[a].x, p_light[a].y, p_light[a].z) - _fragPos);
        vec3 H = normalize(V + L);
        
        // Attenuation for point lights
        float distance = length(vec3(p_light[a].x, p_light[a].y, p_light[a].z) - _fragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = vec3(p_light[a].r, p_light[a].g, p_light[a].b) * attenuation;

        // PBR BRDF terms
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        // Diffuse and specular contributions
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        // Final color contribution
        float NdotL = max(dot(N, L), 0.0);
        result += (kD * albedo / 3.14159265359 + specular) * radiance * NdotL;
    }

    // -------------------------------------------------------------------------
    // Spot Light Calculations
    // -------------------------------------------------------------------------
    for(int a = 0; a < s_count; a++) {
        vec3 light_pos = vec3(s_light[a].x, s_light[a].y, s_light[a].z);
        vec3 target_dir = normalize(vec3(s_light[a].dx, s_light[a].dy, s_light[a].dz));
        vec3 light_dir = normalize(_fragPos - light_pos);
        float cos_val = dot(target_dir, light_dir);

        if (cos_val > cos(radians(s_light[a].angle))) {
            vec3 L = -light_dir;
            vec3 H = normalize(V + L);
            
            // Attenuation for spot lights
            float distance = length(light_pos - _fragPos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = vec3(s_light[a].r, s_light[a].g, s_light[a].b) * attenuation;

            // PBR BRDF terms
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

            // Diffuse and specular contributions
            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;
            
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            // Final color contribution
            float NdotL = max(dot(N, L), 0.0);
            result += (kD * albedo / 3.14159265359 + specular) * radiance * NdotL;
        }
    }

    // -------------------------------------------------------------------------
    // Finalizing Color
    // -------------------------------------------------------------------------
    // Add ambient lighting contribution
    vec3 ambient = vec3(0.03) * albedo * ao; // A simple ambient term
    vec3 finalColor = ambient + result;

    // Apply tonemapping and gamma correction
    finalColor = finalColor / (finalColor + vec3(1.0));
    float gamma = 2.2f;
    final_color = vec4(pow(finalColor, vec3(1.0 / gamma)), 1.f);
}
)";


static const char* default_fs = R"(
#version 460 core

// INPUTS : 
in vec2 _ftexcoords;
in vec3 _fnormal;
in vec3 _ftangent;
in vec3 _fbittangent;
in vec3 _fragPos;


// Directional Light : 
struct D_LIGHT {
	float x,y,z;
	float r,g,b;
};

layout (std430, binding = 0) buffer d_lights {
	int d_count;
	D_LIGHT d_light[];
};

//Point Light : 
struct P_LIGHT {
	float x,y,z;
	float r,g,b;
};

layout (std430, binding = 1) buffer p_lights {
	int p_count;
	P_LIGHT p_light[];
};

// Spot Light : 
struct S_LIGHT {
	float x,y,z;
	float r,g,b;
	float dx,dy,dz;
	float angle;
};

layout (std430, binding = 2) buffer s_lights {
	int s_count;
	S_LIGHT s_light[];
};


//Light Maps : 
layout (binding = 0) uniform sampler2D diffusemap;		
layout (binding = 1) uniform sampler2D normalmap;

// OUTPUTS : 
out vec4 final_color;		

// MAIN 
void main() {

// Calculating the normal : 
vec3 normVector = vec3(0,0,0);
mat3 TBN = mat3(normalize(_ftangent),normalize(_fbittangent),normalize(_fnormal));

if(texture(normalmap,_ftexcoords).rgb == vec3(0,0,0)) 
normVector = _fnormal;

else {
normVector = TBN * (texture(normalmap,_ftexcoords).rgb * 2.0 - 1.0);
}

// LIGHTING CALCULATIONS : 

vec3 result = vec3(0,0,0);
vec3 diffuseColor = pow(texture(diffusemap,_ftexcoords).rgb, vec3(2.2));

// Directional Light Calculations : 

for(int a = 0;a < d_count;a++) {
// Diffuse 
	vec3 lightvector = normalize(-vec3(d_light[a].x, d_light[a].y, d_light[a].z));
	float diffsth = max(dot(normVector,lightvector),0.0);
	vec3 diffres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * diffsth * diffuseColor;
	result += diffres;
}		

// Point Light Calculations : 

for(int a = 0;a < p_count;a++) {

// Diffuse
	vec3 lightvector = normalize(vec3(p_light[a].x, p_light[a].y, p_light[a].z) - _fragPos);
	float diffsth = max(dot(normVector,lightvector),0.0);
	vec3 diffres = vec3(p_light[a].r, p_light[a].g, p_light[a].b) * diffsth * diffuseColor;
	result += diffres;
}

// Spot Light Calculations : 

for(int a = 0;a < s_count;a++) {
	vec3 light_pos = vec3(s_light[a].x,s_light[a].y,s_light[a].z);
	vec3 target_dir = normalize(vec3(s_light[a].dx, s_light[a].dy, s_light[a].dz));
	vec3 light_dir = normalize(_fragPos - light_pos);
	float cos_val = dot(target_dir, light_dir);
	if(cos_val > cos(radians(s_light[a].angle))) {
// Diffuse 
		vec3 lightvector = -light_dir;
		float diffsth = max(dot(normVector,lightvector),0.0);
		vec3 diffres = vec3(s_light[a].r, s_light[a].g, s_light[a].b) * diffsth * diffuseColor;
		result += diffres;
}
}

float gamma = 2.2f;
final_color = vec4(pow(result,vec3(1.0/gamma)),1.f);

}
)";


		// SHADERS FOR NORMAL RENDERING : 

		static const char* normal_vs =
			"#version 460 core\n"
			"layout(location = 0) in vec3 ver_pos;\n"
			"layout(location = 3) in vec3 ver_normal;\n"

			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 normalMatrix;\n"

			"out vec4 norm_tip;\n"

			"void main() {\n"
			"vec3 norm = normalize(mat3(normalMatrix) * ver_normal);\n"
			"norm_tip = projectionMatrix * viewMatrix * vec4(norm + vec3(modelMatrix * vec4(ver_pos, 1.0)), 1.0);\n"
			"gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);\n"
			"}\n"
			;

		static const char* normal_gs =
			"#version 460 core\n"

			"layout (triangles) in;\n"
			"layout (line_strip, max_vertices = 12) out;\n"

			"in vec4 norm_tip[];\n"
			"out vec3 col;\n"

			"void main() {\n"

			//Render Normals : 
			"col = vec3(1.0, 1.0, 0.0);\n"

			"gl_Position = gl_in[0].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[0];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"

			"gl_Position = gl_in[1].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[1];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"

			"gl_Position = gl_in[2].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[2];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"
			"}\n"
			;

		static const char* normal_fs =
			"#version 460 core\n"
			"in vec3 col;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"frag_color = vec4(col, 1.0);\n"
			"}\n"
			;

		// SHADERS FOR RENDERING MESHES

		static const char* mesh_vs =
			"#version 460 core\n"
			"layout(location = 0) in vec3 ver_pos;\n"
			"layout(location = 3) in vec3 ver_normal;\n"

			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 normalMatrix;\n"			

			"void main() {\n"		
			"gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);\n"
			"}\n"
			;

		static const char* mesh_gs =
			"#version 460 core\n"

			"layout (triangles) in;\n"
			"layout (line_strip, max_vertices = 6) out;\n"

			"void main() {\n"
				"gl_Position = gl_in[0].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[1].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"

				"gl_Position = gl_in[1].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[2].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"

				"gl_Position = gl_in[0].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[2].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"
			"}\n"
			;

		static const char* mesh_fs =
			"#version 460 core\n"
			"in vec3 col;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"frag_color = vec4(0.0, 1.0, 0.0, 1.0);\n"
			"}\n"
			;



	class Shader {
	private:
		unsigned int program;
		unsigned int vshader; // vertex shader
		unsigned int fshader; // fragment shader
		unsigned int gshader; // geometry shader
	public:

		void create();	
		void destroy();

		void load_from_buffer(const char* vertex_shader, const char* fragment_shader,const char* geometry_shader = nullptr);
		void load_from_file(const char* vertex_shader_path,const char* fragment_shader_path,const char* geometry_shader_path = nullptr);

		void use();

		void uniformmat4f(const char* name, glm::mat4 val);
		void uniformfloat(const char* name, float val);
		void uniformvec3(const char* name, float x, float y, float z);
		void uniformint(const char* name, int v);

	};


}
#version 330 core

out vec4 FragColor;

struct Environment {
	float ambientStrength;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	
	float shininess;
	float specularStrength;
};

struct DirLight {
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform vec3 viewPos;
uniform Environment environ;
uniform Material material;
uniform vec3 objectColor;

uniform PointLight pointLight;
uniform DirLight dirLight;

uniform bool diffuseStatus;
uniform bool normalStatus;

uniform sampler2D normTexture;
uniform sampler2D diffTexture;


// functions:
vec3 CalcDirLight(DirLight light, vec3 normal);
vec3 CalcPointLight(PointLight light, vec3 normal);

void main(){
	// Set normals:
	vec3 norm ;
	if(normalStatus){	
		norm = texture(normTexture, TexCoord).rgb;
		norm = normalize(norm * 2.0 - 1.0);   
		norm = normalize(TBN * norm);
	}
	else {
		norm = normalize(Normal);
	}
	
	// Add lights:
	vec3 dlight = CalcDirLight(dirLight, norm);
	vec3 plight = CalcPointLight(pointLight, norm);
	vec3 allLights = dlight + plight;
	
	vec4 LightColor = vec4(allLights * objectColor, 1.0f);
	vec4 TextureColor = texture(diffTexture, TexCoord);
	
	if(diffuseStatus) {
		FragColor = TextureColor * LightColor;
	}
	else {
		FragColor = LightColor;
	}
}

vec3 CalcDirLight(DirLight light, vec3 normal){
	// Light direction:
	vec3 lightDir = normalize(-light.direction);
	
	// View direction:
	vec3 viewDir = normalize(viewPos - FragPos);

	// Ambient:
	vec3 ambient = light.ambient * environ.ambientStrength;
	
    // Diffuse shading:
    float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff;
	
    // Specular shading:
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * material.specularStrength * spec;
	
    // Combine results:
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal){
	// Light direction:
	vec3 lightDir = normalize(light.position - FragPos);
	
	// View direction:
	vec3 viewDir = normalize(viewPos - FragPos);
	
	// Ambient:
	vec3 ambient = light.ambient * environ.ambientStrength;
	
	// Diffuse light:
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff;
	
	// Specular light:
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfway = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfway, normal), 0.0), material.shininess);
	vec3 specular = light.specular * material.specularStrength * spec;
	
	// Combine results:
	return (ambient + diffuse + specular);
}
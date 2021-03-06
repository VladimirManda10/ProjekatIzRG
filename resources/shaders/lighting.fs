#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct PointLight{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLight;
uniform DirectionalLight directionalLight;

vec3 CalcDirLight(DirectionalLight directionalLight,vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight pointLight,vec3 normal,vec3 fragPos,vec3 viewDir);
void main(){
	
	
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 result  = CalcDirLight(directionalLight,normal,viewDir);
	result += CalcPointLight(pointLight,normal,FragPos,viewDir);
	FragColor = vec4(result,1.0);
}


vec3 CalcPointLight(PointLight pointLight,vec3 normal,vec3 FragPos,vec3 viewDir){
	//difuzna komponenta
	vec3 lightDir = normalize(pointLight.position - FragPos);
	float diff = max(dot(normal,lightDir),0.0);
	//spekularna komponenta
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	//atenuacija	
	float distance = length(pointLight.position - FragPos);
	float attenuation = 1.0/(pointLight.constant + pointLight.linear*distance + pointLight.quadratic*(distance*distance));
	
	vec3 ambient = pointLight.ambient*vec3(texture(material.diffuse,TexCoords));
	vec3 diffuse = pointLight.diffuse*diff*vec3(texture(material.diffuse,TexCoords));
	vec3 specular = pointLight.specular*spec*vec3(texture(material.specular,TexCoords));
	ambient*=attenuation;
	diffuse*=attenuation;
	specular*=attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirectionalLight directionalLight,vec3 normal,vec3 viewDir){
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(normal,lightDir),0.0);
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	
	vec3 ambient = directionalLight.ambient*vec3(texture(material.diffuse,TexCoords));
	vec3 diffuse = directionalLight.diffuse*diff*vec3(texture(material.diffuse,TexCoords));
	vec3 specular = directionalLight.specular*spec*vec3(texture(material.specular,TexCoords));
	return (ambient + diffuse + specular);
}

#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    float shininess;

};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
  
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
      
	float constant;
	float linear;
	float quadratic;
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

#define NR_POINT_LIGHTS 2

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform float lightIntensity;


// function prototypes
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 tex);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 texDif, vec4 texSpec);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 texDif, vec4 texSpec);

void main()
{    
	vec3 result = vec3(0.0);

	// output += CalcPointLight()
	vec4 texDif = texture(material.texture_diffuse1, TexCoords);
	vec4 texSpec = texture(material.texture_specular1, TexCoords);
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
  
	// == =====================================================
	// Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
	// For each phase, a calculate function is defined that calculates the corresponding color
	// per lamp. In the main() function we take all the calculated colors and sum them up for
	// this fragment's final color.
	// == =====================================================
  
	// phase 1: directional lighting
	//vec3 result = CalcDirLight(dirLight, norm, viewDir, tex);
  
	// phase 2: point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texDif, texSpec);    
  
	// phase 3: spot light
	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir, texDif, texSpec);    
  
	FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 tex)
//{
    //vec3 lightDir = normalize(-light.direction);
    //// diffuse shading
    //float diff = max(dot(normal, lightDir), 0.0);
    //// specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
    //// combine results
    //vec3 ambient = light.ambient * material.ambient * vec3(tex);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse) * vec3(tex);
    //vec3 specular = light.specular * (spec * material.specular) * vec3(tex);
    //return (ambient + diffuse + specular);
//}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 texDif, vec4 texSpec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 * lightIntensity / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// combine results
    vec3 ambient = light.ambient * material.ambient * vec3(texDif);
    vec3 diffuse = light.diffuse * (diff * material.diffuse) * vec3(texDif);
    vec3 specular = light.specular * spec * vec3(texSpec);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 texDif, vec4 texSpec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
	// combine results
    vec3 ambient = light.ambient * vec3(texDif);
    vec3 diffuse = light.diffuse * diff * texDif.rgb;
    vec3 specular = light.specular * spec * texSpec.rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

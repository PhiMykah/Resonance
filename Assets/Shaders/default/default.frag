#version 460 core
out vec4 FragColor;


in vec3 currPos; // Input object's current position from vert shader to frag shader
in vec3 Normal; // Input normal vectors from vert shader to frag shader
in vec3 color; // Inputs color from vert shader to frag shader
in vec2 texCoords; // Input texture coordinate mapping from vert shader to frag shader
uniform sampler2D diffuse0; // Obtain albedo texture unit from main function
uniform sampler2D specular0; // Obtain specular texture unit from main function
uniform vec4 lightColor; // Color obtained from the light source
uniform vec3 lightPos; // Position of the light source
uniform vec3 camPos; // Obtain camera position for specular lighting

// Material properties of object
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material; 

// Directional lighting properties
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

vec4 CalcPointLight();
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  

void main()
{ 
   // Final color output
   FragColor = CalcPointLight();

   // Fog distance effect
	// float depth = logisticDepth(gl_FragCoord.z);
	// FragColor = directionalLight() * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}


// Point light implements a light that emanates in all directions
// with a relaitionship between distance and intensity.
// The equation Intensity(d) = 1/(a*d^2+b*d+1)
vec4 CalcPointLight(){
   // Vector of light to target position
   vec3 lightVec = lightPos - currPos;
   
   // Calculate distance that light travels by length of vector
   float dist = length(lightVec);

   // Constants for equation
   float a = 1.0f; // Controls how fast the intensity decreases
   float b = 0.04f; // Controls how far the light reaches

   // Calculate intensity from equation above
   float intensity = 1.0f / ((a * dist * dist) + (b * dist) + 1.0f);

   // Create ambient light for the object
   float ambient = 0.2f;

   // Normalize normal vector and obtain light direction vector
   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightVec);

   // Current light implementation: Diffuse Lighting
   float diffuse = max(dot(normal, lightDirection), 0.0f);

   // Calculate specular light
   float specular = 0.0f;
   if (diffuse != 0.0f) 
   {
      float specularLight = 0.5f;
      vec3 viewDirection = normalize(camPos - currPos);
      vec3 reflectionDirection = reflect(-lightDirection, normal);

      vec3 halfwayVec = normalize(viewDirection + lightDirection);

      float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
      specular = specAmount * specularLight;
   }

   // // Ignore fragments that contain alpha values less than 0.1
   // if (texture(diffuse0, texCoords).a < 0.1)
   //    discard;

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoords) * (diffuse * intensity + ambient) + texture(specular0, texCoords).r * specular * intensity) * lightColor;
}

// Directional light is light where the source is considered so far away
// that the light rays are all parallel with each other.
// This kind of light is commonly seen with sun rays.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
   vec3 lightDir = normalize(-light.direction);
   // diffuse shading
   float diff = max(dot(normal, lightDir), 0.0);
   // specular shading
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   // combine results
   vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
   vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
   vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
   return (ambient + diffuse + specular);
}

vec4 directionalLight(){
   // Light ray direction (points in opposite direction of light source)
   vec3 lightVec = vec3(1.0f, 1.0f, 0.0f);

   // Create ambient light for the object
   float ambient = 0.2f;

   // Normalize normal vector and obtain light direction vector
   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightVec);

   // Current light implementation: Diffuse Lighting
   float diffuse = max(dot(normal, lightDirection), 0.0f);

   // Calculate specular light
   float specularLight = 0.5f;
   vec3 viewDirection = normalize(camPos - currPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
   float specular = specAmount * specularLight;

   // // Ignore fragments that contain alpha values less than 0.1
   // if (texture(diffuse0, texCoords).a < 0.1)
   //    discard;

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoords) * (diffuse + ambient) + texture(specular0, texCoords).r * specular) * lightColor;
}

// Spot light will light up a conic area emitting from the source
// similar to that of a desk lamp, flashlight, or evidently a spotlight
vec4 spotLight(){
   // Cone cosine values between light direction and cone vectors
   // This is more efficient than writing in terms of the angles as the cosine needs to be calculated
   float outerCone = 0.9f; // Angle cosine of outer cone
   float innerCone = 0.95f; // Angle cosine of inner cone

   // Light ray direction
   vec3 lightVec = lightPos - currPos;

   // Create ambient light for the object
   float ambient = 0.2f;

   // Normalize normal vector and obtain light direction vector
   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightVec);

   // Current light implementation: Diffuse Lighting
   float diffuse = max(dot(normal, lightDirection), 0.0f);

   // Calculate specular light
   float specularLight = 0.5f;
   vec3 viewDirection = normalize(camPos - currPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
   float specular = specAmount * specularLight;

   // Fidn the angle between the current point and the central light direction
   float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
   // Obtain intensity by clamping angle to the outer and inner cones
   float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

   // // Ignore fragments that contain alpha values less than 0.1
   // if (texture(diffuse0, texCoords).a < 0.1)
   //    discard;

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoords) * (diffuse * intensity + ambient) + texture(specular0, texCoords).r * specular * intensity) * lightColor;
}

float near = 0.1f;
float far = 100.f;

// Make depth buffer operate on a linear function
float linearizeDepth(float depth)
{
   return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

// Create fog effect as the view leaves the depth buffer threshold
float logisticDepth(float depth, float steepness, float offset){
   float zVal = linearizeDepth(depth);

   return (1 / (1 + exp(-steepness * (zVal - offset))));
}

float logisticDepth(float depth){
    return logisticDepth(depth, 0.5f, 5.0f);
}

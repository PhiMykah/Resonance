#version 460 core
out vec4 FragColor;

// Input object's current position from vert shader to frag shader
in vec3 currPos;

// Input normal vectors from vert shader to frag shader
in vec3 Normal;

// Inputs color from vert shader to frag shader
in vec3 color;

// Input texture coordinate mapping from vert shader to frag shader
in vec2 texCoord;

// Obtain albedo texture unit from main function
uniform sampler2D diffuse0;

// Obtain specular texture unit from main function
uniform sampler2D specular0;

// Color obtained from the light source
uniform vec4 lightColor;

// Position of the light source
uniform vec3 lightPos;

// Obtain camera position for specular lighting
uniform vec3 camPos;

// Point light implements a light that emanates in all directions
// with a relaitionship between distance and intensity.
// The equation Intensity(d) = 1/(a*d^2+b*d+1)
vec4 pointLight(){
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
   float specularLight = 0.5f;
   vec3 viewDirection = normalize(camPos - currPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
   float specular = specAmount * specularLight;

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

// Directional light is light where the source is considered so far away
// that the light rays are all parallel with each other.
// This kind of light is commonly seen with sun rays.
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

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
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

   // Final output color 
   //      Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

void main()
{ 
   FragColor = pointLight();
}
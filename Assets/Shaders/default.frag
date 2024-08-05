#version 460 core
out vec4 FragColor;

// Inputs color from vert shader to frag shader
in vec3 color;

// Input texture coordinate mapping from vert shader to frag shader
in vec2 texCoord;

// Input normal vectors from vert shader to frag shader
in vec3 Normal;

// Input object's current position from vert shader to frag shader
in vec3 currPos;

// Obtain albedo texture unit from main function
uniform sampler2D tex0;

// Obtain specular texture unit from main function
uniform sampler2D tex1;

// Color obtained from the light source
uniform vec4 lightColor;

// Position of the light source
uniform vec3 lightPos;

// Obtain camera position for specular lighting
uniform vec3 camPos;

void main()
{ 
   // Create ambient light for the object
   float ambient = 0.2f;

   // Normalize normal vector and obtain light direction vector
   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightPos - currPos);

   // Current light implementation: Diffuse Lighting
   float diffuse = max(dot(normal, lightDirection), 0.0f);

   // Calculate specular light
   float specularLight = 0.5f;
   vec3 viewDirection = normalize(camPos - currPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
   float specular = specAmount * specularLight;

   // Final output color 
   //           Primary Texture Color based on ambient and Diffused light + Specular map using texture 
   FragColor = (texture(tex0, texCoord) * (diffuse + ambient) + texture(tex1, texCoord).r * specular) * lightColor;
}
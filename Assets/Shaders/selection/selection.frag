#version 460 core
out uvec3 fragColor;

uniform uint objID;

void main() {
    // Output the object ID as an RGB integer
    fragColor = uvec3(objID, gl_PrimitiveID + 1, 0);  // Encode object ID in red channel
}

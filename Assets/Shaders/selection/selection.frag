#version 460 core
out uvec3 fragColor;

uniform uint objID;
flat in uint primID;

void main() {
    // Output the object ID as an RGB integer
    fragColor = uvec3(objID, primID + 1, 0);  // Encode object ID in red channel
}

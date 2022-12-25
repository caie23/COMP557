#version 410 core
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 MinvT; 
uniform mat4 LightPV;

in vec4 aPos;
in vec3 aNor;

out vec3 positionForFP;
out vec3 normalForFP;
out vec4 positionLightCVV;

void main() {
    vec4 Mpos = M * aPos;
    // positionLightCVV = vec4(0); //TODO: compute positionLightCVV
    positionLightCVV = P * Mpos;

    vec4 normalDirection = MinvT * vec4(  aNor, 0 );
    normalForFP = (V * vec4(normalize( normalDirection.xyz ),0)).xyz;
    vec4 VMpos = V * Mpos;
    positionForFP = VMpos.xyz;
    
    gl_Position = P * VMpos; 
}

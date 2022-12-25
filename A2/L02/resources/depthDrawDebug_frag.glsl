#version 410 core
uniform sampler2D depthMap; 

in vec2 texCoordForFP;

out vec4 fragColor;

void main(void) {
    vec4 c = texture( depthMap, texCoordForFP.xy);
    fragColor = vec4( c.rrr, 0.5 );
}
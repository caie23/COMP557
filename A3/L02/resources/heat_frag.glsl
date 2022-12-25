#version 410 core

in vec3 camSpacePosition;
in vec3 camSpaceNormal;
in float utv;
in float phiv;

out vec4 out_fragColor;

uniform vec3 lightCamSpacePosition;
uniform vec3 lightColor;
uniform vec3 materialDiffuse;
uniform float materialShininess; 

void main(void) {
	
	vec3 v = normalize(-camSpacePosition);
	vec3 n = normalize(camSpaceNormal);
	vec3 l = normalize(lightCamSpacePosition - camSpacePosition);

	// TODO: 4, 7, 11 Implement your GLSL per fragement lighting, heat colouring, and distance stripes here!
    vec3 diffuse = materialDiffuse * max(0.0, dot(n, l));
    vec3 h = normalize(l + v); // halfway vector
    vec3 specular = lightColor * pow(max(0.0, dot(n, h)), materialShininess);
    vec3 lighting = diffuse + specular;
    vec3 heat = vec3(1,0,0);
    
    //vec3 basecolor = n.xyz * 0.5; // colorful
    vec3 basecolor = vec3(0, 0, 1);
    vec3 result = (basecolor+lighting)*(1-utv) + heat*utv;
    
    out_fragColor =  clamp( vec4( result , 1.0 ), 0, 1);
    

	// can use this to initially visualize the normal	
    //out_fragColor =  clamp( vec4( n.xyz * 0.5 + vec3( 0.5, 0.5,0.5 ), 0.5 + utv + phiv ), 0, 1 );
    //out_fragColor =  clamp( vec4( n.xyz * 0.5 + vec3( 0.5, 0.5,0.5 ), 0.8 + clamp(utv + phiv, 0, 0) ), 0, 1);
}

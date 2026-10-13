#version 330 core

out vec4 FragColor;

in vec2 localPosition;

uniform vec3 particleColor;
uniform float particleHeight;
uniform float particleFade;
uniform float particleFlicker;

void main()
{
    float distanceFromCenter = length(localPosition);

    //this the code that remove the edges and make it a circle
    if(distanceFromCenter > 0.5)
    {
        discard;
    }

    float glow = 1.0 - (distanceFromCenter / 0.5);

    glow = max(glow, 0.0);

    glow = pow(glow, 1.5);

    float heightFactor = clamp(particleHeight, 0.0, 1.0);

    vec3 bottomColor = vec3(1.0, 0.85, 0.2);
    vec3 topColor = vec3(0.9, 0.08, 0.01);

    vec3 fireColor = mix(bottomColor, topColor, heightFactor);

    FragColor = vec4(fireColor * particleColor * particleFlicker, glow * particleFade);
}
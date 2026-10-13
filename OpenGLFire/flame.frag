
#version 330 core

in vec2 uv;

out vec4 FragColor;

uniform float time;

// This is a simple procedural noise building block, not a complete noise algorithm.
float hash(float value)
{
    return fract(sin(value * 127.1) * 43758.5453);
}

float smoothNoise(float value)
{
    float cell = floor(value);
    float fraction = fract(value);

    fraction = fraction * fraction * (3.0 - 2.0 * fraction);

    float a = hash(cell);
    float b = hash(cell + 1.0);

    return mix(a, b, fraction);
}

void main()
{
    float height = uv.y;

    float turbulence = smoothNoise(height * 12.0 + time * 2.0);

    float center = 0.5 + 0.06 * sin(height * 8.0 + time * 2.0) + 0.025 * sin(height * 17.0 - time * 3.0) + (turbulence - 0.5) * 0.035;

    float width = 0.32 * pow(1.0 - height, 0.7);

    width *= 0.9 + 0.1 * sin(height * 10.0 + time * 4.0);
    width *= 0.95 + turbulence * 0.1;
    width *= 0.75 + 0.25 * sin(height * 12.0 + 1.0);

    float distanceFromCenter = abs(uv.x - center);

    float innerTurbulence = smoothNoise( height * 24.0 - time * 3.5 + distanceFromCenter * 18.0);

    float edge = width - distanceFromCenter;

    float flameMask = smoothstep(0.0, 0.06, edge);

    if (flameMask < 0.01)
        discard;

    vec3 red = vec3(0.8, 0.025, 0.002);
    vec3 orange = vec3(1.0, 0.16, 0.005);
    vec3 yellow = vec3(1.0, 0.75, 0.12);
    vec3 hotCore = vec3(1.0, 0.95, 0.65);

    float coreWidth = width * 0.45;

    float coreMask = 1.0 - smoothstep(coreWidth * 0.35, coreWidth, distanceFromCenter);

    float heightColor = smoothstep(0.0, 0.75, height);

    float hotness = clamp(coreMask * 0.85 + (innerTurbulence - 0.5) * 0.35, 0.0, 1.0);

    vec3 color = mix(red, orange, heightColor);
    color = mix(color, yellow, hotness);
    color = mix(color, hotCore, hotness * 0.35);

    float tipFade = 1.0 - smoothstep(0.65, 1.0, height);
    float alpha = flameMask * tipFade;

    FragColor = vec4(color, alpha);
}
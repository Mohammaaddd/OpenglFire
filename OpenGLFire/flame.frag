
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
    float animatedHeight = uv.y + 0.012 * sin(time * 7.0 + uv.x * 8.0);

    float height = clamp(animatedHeight, 0.0, 1.0);

    float turbulence = smoothNoise(height * 12.0 + time * 2.0);


    float wave1 = sin(height * 10.0 - time * 3.5);
    float wave2 = sin(height * 17.0 + time * 4.8);
    float wave3 = sin(height * 25.0 - time * 6.2);

    float center = 0.5 + 0.035 * wave1 + 0.018 * wave2 + 0.008 * wave3;

    float baseWidth = 0.34 * pow(1.0 - height, 0.65);

    float tongues = 0.04 * wave1 + 0.025 * wave2 + 0.012 * wave3;

    float width = baseWidth + tongues * (0.3 + height);
    width = max(width, 0.005);

    float distanceFromCenter = abs(uv.x - center);

    float innerTurbulence = smoothNoise( height * 24.0 - time * 3.5 + distanceFromCenter * 18.0);

    float edge = width - distanceFromCenter;

    float flameMask = smoothstep(0.0, 0.06, edge);

    if (flameMask < 0.01)
        discard;


    vec3 darkRed = vec3(0.65, 0.015, 0.002);
    vec3 red = vec3(1.0, 0.045, 0.002);
    vec3 orange = vec3(1.0, 0.24, 0.008);
    vec3 yellow = vec3(1.0, 0.72, 0.12);
    vec3 hotCore = vec3(1.0, 0.94, 0.65);

    float baseGlow = 1.0 - smoothstep(0.0, 0.4, height);

    vec3 color = mix(darkRed, orange, baseGlow);
    color = mix(color, red, smoothstep(0.0, 0.65, height));

    float coreWidth = width * 0.48;

    float coreMask = 1.0 - smoothstep(
        coreWidth * 0.25,
        coreWidth,
        distanceFromCenter
    );

    float innerFlicker = 0.85 + 0.10 * sin(time * 11.0 + height * 19.0) + 0.05 * sin(time * 17.0 - height * 13.0);

    color = mix(color, yellow, coreMask * innerFlicker);
    color = mix(color, hotCore, coreMask * coreMask * 0.35);
    float tipFade = 1.0 - smoothstep(0.65, 1.0, height);
    float alpha = flameMask * tipFade;

    FragColor = vec4(color, alpha);
}
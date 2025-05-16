#version 330 core

in float vLifeRatio;   // normalized lifespan from 0.0 (new) to 1.0 (old)
out vec4 fragColor;

void main() {
    // Clamp the life ratio just for safety.
    float life = clamp(vLifeRatio, 0.0, 1.0);
    
    // Define the colors you want:
    vec3 startColor = vec3(1.0, 0.5, 0.0); // bright orange for new particles
    vec3 endColor   = vec3(0.7, 0.1, 0.1); // dark red for old particles
    
    // Mix the colors based solely on life ratio.
    vec3 mixedColor = mix(startColor, endColor, life);
    
    // For debugging, force alpha = 1 and ignore the texture and vColor.
    fragColor = vec4(mixedColor, 1.0);
}

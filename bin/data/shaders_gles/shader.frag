#version 330 core

in vec4 vColor;
in float vLifeRatio;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 texColor = texture(tex, gl_PointCoord);
    
    // Define color gradient: from bright orange to dark red
    vec3 startColor = vec3(1.0, 0.5, 0.0); // Bright orange
    vec3 endColor = vec3(0.5, 0.0, 0.0);   // Dark red

    // Interpolate color based on life ratio
    vec3 explosionColor = mix(startColor, endColor, vLifeRatio);

    fragColor = texColor * vec4(explosionColor, 1.0) * vColor;
}

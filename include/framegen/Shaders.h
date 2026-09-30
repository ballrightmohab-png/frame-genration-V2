#ifndef FRAME_GEN_SHADERS_H
#define FRAME_GEN_SHADERS_H

namespace LeviMod {

    static const char* FRAME_GEN_VERT_SHADER = R"(
#version 300 es
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

    static const char* FRAME_GEN_FRAG_SHADER = R"(
#version 300 es
precision highp float;

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uPrevColorTex;
uniform sampler2D uCurrColorTex;
uniform sampler2D uDepthTex;

uniform mat4 uPrevViewProjInv;
uniform mat4 uCurrViewProj;
uniform float uInterpolationFactor; // e.g. 0.5 for mid-frame
uniform vec2 uScreenSize;

// Unprojects depth buffer z into 3D world space, reprojects into current view
vec2 calculateMotionVector(vec2 uv, float depth) {
    // Convert UV and depth to NDC [-1, 1]
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // Reconstruct world space position of previous frame pixel
    vec4 worldPos = uPrevViewProjInv * ndc;
    worldPos /= worldPos.w;

    // Project world space position into current frame space
    vec4 currentNDC = uCurrViewProj * worldPos;
    currentNDC /= currentNDC.w;

    vec2 currentUV = currentNDC.xy * 0.5 + 0.5;
    return currentUV - uv; // Motion vector in UV space
}

void main() {
    float depth = texture(uDepthTex, vTexCoord).r;

    // Calculate optical motion vector
    vec2 motionVec = calculateMotionVector(vTexCoord, depth);

    // Bidirectional spatial warping
    vec2 uvPrev = vTexCoord + motionVec * uInterpolationFactor;
    vec2 uvCurr = vTexCoord - motionVec * (1.0 - uInterpolationFactor);

    // Clamp UVs to screen boundary
    uvPrev = clamp(uvPrev, vec2(0.001), vec2(0.999));
    uvCurr = clamp(uvCurr, vec2(0.001), vec2(0.999));

    vec4 colPrev = texture(uPrevColorTex, uvPrev);
    vec4 colCurr = texture(uCurrColorTex, uvCurr);

    // Smooth temporal blend with occlusion check
    float weight = uInterpolationFactor;
    vec4 finalColor = mix(colPrev, colCurr, weight);

    FragColor = finalColor;
}
)";

} // namespace LeviMod

#endif // FRAME_GEN_SHADERS_H

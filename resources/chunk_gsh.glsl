#version 330 core
layout (points) in;
layout (points, max_vertices = 4) out;

// 0000_0000 000v_vvvu uuuc_basz zzyy_yxxx
// xxx - position.x [0, 8)
// yyy - position.y [0, 8)
// zzz - position.z [0, 8)
// s - is normal pointing in positive direction [0, 1];
// a - is normal alligned with x [0, 1]
// b - is normal alligned with y [0, 1]
// c - is normal alligned with z [0, 1]
// uuuu - texture atlas x [0, 16)
// vvvv - texture atlas y [0, 16)
in uint pack[];

out vec2 texCoord;

void main() {
    uint p = pack[0];
    vec3 position = vec3(
        float(p & 0x8u),
        float((p >> 3) & 0x8u),
        float((p >> 6) & 0x8u));
    uint sign = (p >> 9) & 0x1u;
    uint x = (p >> 10) & 0x1u;
    uint y = (p >> 11) & 0x1u;
    uint z = (p >> 12) & 0x1u;

    float rgt = float(x &  sign);
    float lft = float(x & ~sign);
    float top = float(y &  sign);
    float bot = float(y & ~sign);
    float fwd = float(z &  sign);
    float bwd = float(z & ~sign);

    vec2 texCoordBase = vec2(
        float((p >> 13) & 0xF),
        float((p >> 17) & 0xF)) / 16.0;

    // Rebuild the face quad of the block using unholy magic.
    gl_Position = position + vec3(
        rgt + bwd, 
        top, 
        rgt + top + fwd);
    texCoord = texCoordBase;
    EmitVertex();

    gl_Position = position + vec3(
        rgt + top + bot + fwd, 
        top, 
        lft + top + fwd);
    texCoord = texCoordBase + vec2(1.0, 0.0) / 16.0;
    EmitVertex();

    gl_Position = position + vec3(
        lft + bwd,
        rgt + lft + top + fwd,
        lft + bot + fwd + bwd);
    texCoord = texCoordBase + vec2(0.0, 1.0) / 16.0;
    EmitVertex();

    gl_Position = position + vec3(
        rgt + top + bot + fwd,
        rgt + lft + top + fwd + bwd,
        lft + bot + fwd);
    texCoord = texCoordBase + vec2(1.0, 1.0) / 16.0;
    EmitVertex();

    EndPrimitive();
}  
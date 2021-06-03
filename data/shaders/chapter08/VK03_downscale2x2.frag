//
#version 460

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main()
{
    ivec2 ts = textureSize(texSampler, 0);

    float dx = +1.0 / float(ts.x);
    float dy = -1.0 / float(ts.y);

    vec2 uv = vec2(texCoord.x, 1.0 - texCoord.y);

    vec4 s1 = texture(texSampler, uv + vec2( 0,  0));
    vec4 s2 = texture(texSampler, uv + vec2(dx,  0));
    vec4 s3 = texture(texSampler, uv + vec2( 0, dy));
    vec4 s4 = texture(texSampler, uv + vec2(dx, dy));

    outColor = vec4(((s1 + s2 + s3 + s4) / 4.0).xyz, 1.0);
}

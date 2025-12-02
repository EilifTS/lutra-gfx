[[vk::binding(0, 0)]]
Texture2D texture;

[[vk::binding(1, 0)]]
SamplerState linear_clamp;

struct SpriteInfo
{
    float2 position;
    float2 size;
    float depth;
    uint color;
};
[[vk::binding(2, 0)]]
StructuredBuffer<SpriteInfo> sprite_data;

struct VSInput
{
    uint vertex_id : SV_VertexID;
    uint instance_id : SV_InstanceID;
};

struct VSVarying
{
    float4 position : SV_Position;
    //float2 uv : TexCoord0;
    float4 color : TexCoord1;
    //int texture_id : TexCoord3;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

VSVarying VS(VSInput input)
{
    VSVarying output;
    
    const SpriteInfo sprite_info = sprite_data[input.instance_id];
    const float2 screen_size = float2(800.0, 600.0); /* Hard coded screen size for now */
    const float2 sprite_position = 2.0f * (sprite_info.position - 0.5 * screen_size) / screen_size;
    const float2 sprite_size = sprite_info.size / screen_size;
    const uint4 sprite_color_int = (sprite_info.color.xxxx >> uint4(24, 16, 8, 0)) & uint(0xFF).xxxx;
    const float4 sprite_color = float4(sprite_color_int) / 255.0;
    
    float2 rectangle_offset = float2(0.0, 0.0);
    if (input.vertex_id == 0)
    {
        rectangle_offset = float2(-1.0, -1.0);
    }
    else if (input.vertex_id == 1)
    {
        rectangle_offset = float2(1.0, 1.0);
    }
    else if (input.vertex_id == 2)
    {
        rectangle_offset = float2(1.0, -1.0);
    }
    else if (input.vertex_id == 3)
    {
        rectangle_offset = float2(-1.0, -1.0);
    }
    else if (input.vertex_id == 4)
    {
        rectangle_offset = float2(1.0, 1.0);
    }
    else if (input.vertex_id == 5)
    {
        rectangle_offset = float2(-1.0, 1.0);
    }
    output.position = float4(sprite_position + rectangle_offset * sprite_size, 0.5, 1.0);
    output.color = sprite_color;

    return output;
}

PSOutput PS(VSVarying input)
{
    PSOutput output;

    output.color = input.color;
    //if (input.texture_id != -1)
   // {
    //    output.color *= textures[input.texture_id].Sample(TEXTURE_SAMPLER, input.uv);
    //}

    return output;
}

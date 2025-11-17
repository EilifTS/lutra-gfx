//Texture2D textures[EF_SPRITE_BATCH_SHADER_MAX_TEXTURES] : register(t0, space1);
//SamplerState linear_clamp : register(s0, space0);

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
    if (input.vertex_id == 0)
    {
        output.position = float4(-1.0, -1.0, 0.5, 1.0);
    }
    else if (input.vertex_id == 1)
    {
        output.position = float4(1.0, 1.0, 0.5, 1.0);
    }
    else if (input.vertex_id == 2)
    {
        output.position = float4(1.0, -1.0, 0.5, 1.0);
    }
    else if (input.vertex_id == 3)
    {
        output.position = float4(-1.0, -1.0, 0.5, 1.0);
    }
    else if (input.vertex_id == 4)
    {
        output.position = float4(1.0, 1.0, 0.5, 1.0);
    }
    else if (input.vertex_id == 5)
    {
        output.position = float4(-1.0, 1.0, 0.5, 1.0);
    }
    output.color = float4(1.0, 0.0, 0.0, 1.0);

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

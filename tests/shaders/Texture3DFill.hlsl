[[vk::binding(0, 0)]]
[[vk::image_format("r32f")]]
RWTexture3D<float> output;

[numthreads(4, 4, 4)]
void CS(uint3 id : SV_DispatchThreadID)
{
    uint3 size;
    output.GetDimensions(size.x, size.y, size.z);
    if (all(id < size))
    {
        output[id] = float(id.x + 10 * id.y + 100 * id.z);
    }
}

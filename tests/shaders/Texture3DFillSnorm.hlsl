[[vk::binding(0, 0)]]
[[vk::image_format("r8snorm")]]
RWTexture3D<float> output;

[numthreads(4, 4, 4)]
void CS(uint3 id : SV_DispatchThreadID)
{
    uint3 size;
    output.GetDimensions(size.x, size.y, size.z);
    if (all(id < size))
    {
        output[id] = -1.0 + 2.0 * float(id.x) / float(size.x - 1);
    }
}

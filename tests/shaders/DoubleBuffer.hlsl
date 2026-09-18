[[vk::binding(0, 0)]]
RWStructuredBuffer<uint> data;

[numthreads(1, 1, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    data[id.x] = data[id.x] * 2;
}

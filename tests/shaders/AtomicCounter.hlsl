[[vk::binding(0, 0)]]
RWStructuredBuffer<int> counters;

[numthreads(64, 1, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    InterlockedAdd(counters[0], 1);
    InterlockedAdd(counters[1], int(id.x) + 1);
}

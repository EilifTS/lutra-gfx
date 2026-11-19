[[vk::binding(0, 0)]]
cbuffer Constants
{
    uint4 values;
};

[[vk::binding(1, 0)]]
RWStructuredBuffer<uint> results;

[numthreads(1, 1, 1)]
void CS()
{
    results[0] = values.x;
    results[1] = values.y;
    results[2] = values.z;
    results[3] = values.w;
}

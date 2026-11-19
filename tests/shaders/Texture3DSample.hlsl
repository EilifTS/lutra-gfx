[[vk::binding(0, 0)]]
Texture3D<float> tex;
[[vk::binding(1, 0)]]
SamplerState samp;
[[vk::binding(2, 0)]]
StructuredBuffer<float4> queries;
[[vk::binding(3, 0)]]
RWStructuredBuffer<float> results;

[numthreads(1, 1, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    const float4 query = queries[id.x];
    results[id.x] = tex.SampleLevel(samp, query.xyz, query.w);
}

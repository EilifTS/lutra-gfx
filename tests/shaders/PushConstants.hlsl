struct Constants
{
    uint index;
    uint value;
};

[[vk::push_constant]]
Constants constants;

[[vk::binding(0, 0)]]
RWStructuredBuffer<uint> results;

[numthreads(1, 1, 1)]
void CS()
{
    results[constants.index] = constants.value * 2;
}

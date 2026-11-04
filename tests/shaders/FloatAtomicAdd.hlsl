// Needs VK_EXT_shader_atomic_float (shaderBufferFloat32AtomicAdd), which an app has to request when creating the context
[[vk::ext_capability(6033)]] // AtomicFloat32AddEXT
[[vk::ext_extension("SPV_EXT_shader_atomic_float_add")]]
[[vk::ext_instruction(6035)]] // OpAtomicFAddEXT
float AtomicFAdd([[vk::ext_reference]] float pointer, uint scope, uint semantics, float value);

[[vk::binding(0, 0)]]
RWStructuredBuffer<float> sums;

[numthreads(64, 1, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    // Scope 1 is Device, semantics 0 is relaxed
    AtomicFAdd(sums[0], 1, 0, 0.5f);
    AtomicFAdd(sums[1], 1, 0, float(id.x));
}

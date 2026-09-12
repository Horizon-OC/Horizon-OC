#version 430 core
// CRAWLING IN MY SKINNNNNN
// THESE WOUNDS THEY WILL NOT HEALLLLLLLL
// Furmark bored the fuck out of me tbh although the black hole bench I do want to revisit but thats besides the point. 

// Here we are, this compute shitpost draws more power than furmark.

// TX1 has 2SMs with 4 schedulers with 2x issue. So 256 threads per block, 8 blocks per sm, 2048 threads dispatched as a max
layout(local_size_x = 16, local_size_y = 16) in;   // 256 threads = 8 warps


// uvec4 used here to use LDG, essentially forcing data to be read from the unified instruction cache, diferenciating from a global/generic load 
// For us, besdies the speedup this also means that lanes pulls 512 bytes instead of 128, forcing mem/l2 more.
layout(std430, binding = 0) readonly buffer DataBuffer {
    uvec4 data[];
};

layout(std430, binding = 1) buffer ResultBuffer {
    uint mismatches;
    uint digest;
    uint selfFail;
    uint errCount;
};

// ALU self checks/mismatch storage
layout(std430, binding = 2) buffer ErrorLog {
    uvec4 record[];
};

layout(std430, binding = 3) buffer ExpectedBuffer {
    uint expected[];
};

layout(std140, binding = 0) uniform Params {
    uint n_rows;
    uint n_repeats;
    uint alu_iters;
    uint gather_iters;
    uint max_records;
    uint mode;           // 0 = capture golden, 1 = compare, 2 = ignore
    uint scatter_mask;   // limits size to stay in L2 or not
    uint chain_bias;     // normally 0; see aluChain2
};

const float THRESHOLD = 0.001f;


// "scratchpad", not really. 
// this is configured for 8kb per block (for 64Kb), any higher drops our block count 
shared uint s_scratch[2048];

// FNV-1a. Surpringly heavy because maxwell. (XMAD)
uint hmix(uint h, uint v)
{
    h ^= v;
    h *= 16777619u;
    return h;
}

// Constant shifts, so every index below must be a mask/add
uint xs32(uint x)
{
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

void pushRecord(uint id, uint stage, uint got, uint want)
{
    uint idx = atomicAdd(errCount, 1u);
    if (idx < max_records)
        record[idx] = uvec4(id, stage, got, want);
}


// Man fuck current limits.
// Here we issue 2 instructions per cycle, intervealing to achive this.
// chian_bias is only here to stop the compiler from optimizing it away. 
// CPU side passes zero so results must be equal, otherwise we know an error occured. 
// Giving anything else breaks.
uvec2 aluChain2(uint seed, uint iters)
{
    uint s2 = seed ^ chain_bias;

    uint  h1 = seed,      h2 = s2;
    uint  a1 = seed | 1u, a2 = s2 | 1u;
    float f1 = uintBitsToFloat((seed & 0x007FFFFFu) | 0x3F800000u);
    float f2 = uintBitsToFloat((s2   & 0x007FFFFFu) | 0x3F800000u);

    for (uint i = 0u; i < iters; ++i) {
        // int/FP work
        a1 = xs32(a1) + 0x9E3779B9u;
        a2 = xs32(a2) + 0x9E3779B9u;
        f1 = fma(f1, f1, 0.5);
        f2 = fma(f2, f2, 0.5);
        // Force values back into [1,2].
        f1 = uintBitsToFloat((floatBitsToUint(f1) & 0x007FFFFFu) | 0x3F800000u);
        f2 = uintBitsToFloat((floatBitsToUint(f2) & 0x007FFFFFu) | 0x3F800000u);
        h1 = hmix(h1 ^ a1, floatBitsToUint(f1));
        h2 = hmix(h2 ^ a2, floatBitsToUint(f2));
    }
    return uvec2(h1, h2);
}

// I hate myself
void main()
{
    uint gx  = gl_NumWorkGroups.x * gl_WorkGroupSize.x;
    uint tid = gl_GlobalInvocationID.y * gx + gl_GlobalInvocationID.x;

    // Distance in elements between rows. Also serves as the thread count
    uint stride = gx * gl_NumWorkGroups.y * gl_WorkGroupSize.y;

    uint lid = gl_LocalInvocationIndex;

    uint count = 0u;
    uint h = 2166136261u ^ tid;

    for (uint k = 0u; k < n_repeats; ++k) {

        // read uvec4 data continously
        uvec4 ref = data[tid];
        h = hmix(hmix(hmix(hmix(h, ref.x), ref.y), ref.z), ref.w);

        vec4 fref = uintBitsToFloat(ref);
        uint idx = tid;

        for (uint row = 1u; row < n_rows; ++row) {

            // On reduced load, adss per row instead of IMAD, which because Nvidia would expand into XMAD competing for resorces.
            idx += stride;

            uvec4 v = data[idx];
            h = hmix(hmix(hmix(hmix(h, v.x), v.y), v.z), v.w);

            // FSETP + used instead of creating divergence.
            bvec4 hit = greaterThan(abs(fref - uintBitsToFloat(v)), vec4(THRESHOLD));
            count += uint(hit.x) + uint(hit.y) + uint(hit.z) + uint(hit.w);
        }

        // Gather data in a different line every lane. 
        // Sized under L2 max, because fuck cache
        uint st = xs32(h | 1u);
        for (uint g = 0u; g < gather_iters; ++g) {
            st = xs32(st);
            uvec4 v = data[st & scatter_mask];
            h = hmix(hmix(hmix(hmix(h, v.x), v.y), v.z), v.w);
        }

        // 2x issue redundancy 
        uvec2 cc = aluChain2(h, alu_iters);
        if (cc.x != cc.y) {
            atomicAdd(selfFail, 1u);
            pushRecord(tid, 1u, cc.x, cc.y);
        }
        h = cc.x;


        // Write back to memory
        for (uint j = 0u; j < 8u; ++j)
            s_scratch[lid + j * 256u] = h + j;
        memoryBarrierShared();
        barrier();

        uint t = 0u;
        for (uint j = 0u; j < 8u; ++j)
            t = hmix(t, s_scratch[((lid * 37u + 11u) + j * 251u) & 2047u]);
        h = hmix(h, t);
        memoryBarrierShared();
        barrier();
    }

    // Compare golden samples
    if (mode == 0u) {
        expected[tid] = h;
    } else if (mode == 1u && h != expected[tid]) {
        pushRecord(tid, 2u, h, expected[tid]);
    }


    // Fused reduction, digest/count reduced to 8 barriers with one pass instead of 2.
    s_scratch[lid]         = h;
    s_scratch[256u + lid]  = count;
    memoryBarrierShared();
    barrier();

    for (uint s = 128u; s > 0u; s >>= 1) {
        if (lid < s) {
            s_scratch[lid]        += s_scratch[lid + s];
            s_scratch[256u + lid] += s_scratch[256u + lid + s];
        }
        memoryBarrierShared();
        barrier();
    }

    // Atomic (16 per WGP)
    if (lid == 0u) {
        atomicAdd(digest, s_scratch[0]);
        atomicAdd(mismatches, s_scratch[256u]);
    }
}

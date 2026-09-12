#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 0) readonly buffer DataBuffer {
    double data[];
};

layout(std430, binding = 1) buffer ResultBuffer {
    uint result;
};

layout(std140, binding = 0) uniform Params {
    uint n_rows;
    uint n_repeats;
};

const double THRESHOLD = 1e-3LF;

shared uint s_partial[256];

void main() {
    uint total_cols = gl_NumWorkGroups.x * gl_WorkGroupSize.x
                    * gl_NumWorkGroups.y * gl_WorkGroupSize.y;

    uint col = (gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x))
             + gl_GlobalInvocationID.x;

    uint count = 0u;

    if (n_rows >= 2u) {
        for (uint k = 0u; k < n_repeats; ++k) {
            double bias = double(k) * 1e-12LF;
            double ref = data[col] + bias;

            for (uint row = 1u; row < n_rows; ++row) {
                double val = data[row * total_cols + col];
                if (abs(ref - val) > THRESHOLD)
                    count++;
            }
        }
    }

    uint lid = gl_LocalInvocationIndex;
    s_partial[lid] = count;
    memoryBarrierShared();
    barrier();

    for (uint s = 128u; s > 0u; s >>= 1) {
        if (lid < s)
            s_partial[lid] += s_partial[lid + s];
        memoryBarrierShared();
        barrier();
    }

    if (lid == 0u)
        atomicAdd(result, s_partial[0]);
}

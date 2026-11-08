#pragma once

#include <cmath>
#include <vector>

// Triangle-list meshes (CCW winding, outward normals) in the same flat
// "x y z x y z ..." layout your Object class expects. Both have size 1
// (cube side = 1, sphere diameter = 1) and are centred on the origin.

inline void make_cube_mesh(std::vector<float>& verts, std::vector<float>& normals)
{
    // normal, tangent u, tangent v  (u x v == normal, so the quad is CCW seen from outside)
    static const float F[6][9] = {
        {  1, 0, 0,   0, 0,-1,   0, 1, 0 },
        { -1, 0, 0,   0, 0, 1,   0, 1, 0 },
        {  0, 1, 0,   1, 0, 0,   0, 0,-1 },
        {  0,-1, 0,   1, 0, 0,   0, 0, 1 },
        {  0, 0, 1,   1, 0, 0,   0, 1, 0 },
        {  0, 0,-1,  -1, 0, 0,   0, 1, 0 },
    };
    static const int corner[6][2] = { {-1,-1}, {1,-1}, {1,1}, {-1,-1}, {1,1}, {-1,1} };

    for (const auto& f : F)
        for (const auto& c : corner)
            for (int k = 0; k < 3; k++)
            {
                verts.push_back(0.5f * (f[k] + c[0] * f[3 + k] + c[1] * f[6 + k]));
                normals.push_back(f[k]);
            }
}

inline void make_sphere_mesh(std::vector<float>& verts, std::vector<float>& normals,
                             int stacks = 16, int slices = 32)
{
    const float PI = 3.14159265358979f;
    auto P = [&](int i, int j, float out[3]) {
        float th = PI * i / stacks, ph = 2.f * PI * j / slices;
        out[0] = std::sin(th) * std::cos(ph);
        out[1] = std::cos(th);
        out[2] = std::sin(th) * std::sin(ph);
    };
    auto push = [&](const float p[3]) {
        for (int k = 0; k < 3; k++) { verts.push_back(0.5f * p[k]); normals.push_back(p[k]); }
    };

    for (int i = 0; i < stacks; i++)
        for (int j = 0; j < slices; j++)
        {
            float A[3], B[3], C[3], D[3];
            P(i, j, A); P(i + 1, j, B); P(i + 1, j + 1, C); P(i, j + 1, D);
            push(A); push(C); push(B);
            push(A); push(D); push(C);
        }
}

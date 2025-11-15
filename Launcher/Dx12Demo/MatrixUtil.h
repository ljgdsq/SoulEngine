#pragma once
#include <iostream>
#include <DirectXMath.h>

namespace MatrixUtil
{
    std::ostream &operator<<(std::ostream &os, const DirectX::XMMATRIX &mat)
    {
        DirectX::XMFLOAT4X4 m;
        DirectX::XMStoreFloat4x4(&m, mat);
        for (int i = 0; i < 4; ++i)
        {
            os << "[ ";
            os << m.m[i][0] << " " << m.m[i][1] << " " << m.m[i][2] << " " << m.m[i][3];
            os << " ]\n";
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const DirectX::XMVECTOR &vec)
    {
        DirectX::XMFLOAT4 v;
        DirectX::XMStoreFloat4(&v, vec);
        os << "[ " << v.x << " " << v.y << " " << v.z << " " << v.w << " ]";
        return os;
    }

}
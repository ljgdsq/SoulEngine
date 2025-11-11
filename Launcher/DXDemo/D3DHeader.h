#pragma once


// dx 通用头文件
#include <Windows.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <d3d11.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")


// 可选：防止宏污染
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#pragma once

/* 

Graphics System

 Notes on DirectX

 DirectXMath
	- 3D math library
	- SSE2: Streaming SIMD Extensions 2
	- 128-bit SIMD registers (Single Instruction Multiple Data)
	- further reading: Designing Fast Cross-Platform SIMD Vector Libraries by Oliveira2010

 Must enable SSE2: Project Properties > Configuration Properties > C/C++ > Code Generation > Enable Enhanced Instruction Set
 Must enable the fast floating point model /fp:fast : Project Properties > Configuration Properties > C/C++ > Code Generation > Floating Point Model

 Vectors:
	1. Use XMVECTOR for local or global variables.
	2. Use XMFLOAT2, XMFLOAT3, and XMFLOAT4 for class data members.
	3. Use loading functions to convert from XMFLOAT{n} to XMVECTOR before doing calculations.
	4. Do calculations with XMVECTOR instances.
	5. Use storage functions to convert from XMVECTOR to XMFLOAT{n}.

	SIMD vector: XMVECTOR
	POD vector: XMFLOAT{n}

	Loading Functions: XMLoadFloat{n}
	Storage Functions: XMStoreFunctions{n}

 Matrices:
	...


 Direct3D 12
	Low-level Graphics API to control the GPU from our app

 COM:
	Component Object Model
	Wrapper Interface around all DirectX classes

*/
#include <DirectXMath.h>
#include <DirectXPackedVector.h> // additional data types

void Instructions()
{
	// Check support for SSE2 (Pentium4, AMD K8, and above).
	DirectX::XMVerifyCPUSupport();

	DirectX::XMVectorZero();
	DirectX::XMFLOAT4;

	// DirectX::XMVectorGetX();

	DirectX::FXMVECTOR; // 1, 2, 3
	DirectX::GXMVECTOR; // 4
	DirectX::HXMVECTOR; // 5, 6
	DirectX::CXMVECTOR; // 7...

	// constant XMVECTOR: XMVECTORF32 or XMVECTORU32
	static const DirectX::XMVECTORF32 g_vHalfVector = { 0.5f, 0.5f, 0.5f, 0.5f };
	static const DirectX::XMVECTOR splatVector = DirectX::XMVectorSplatOne();

	// The scalar result is replicated in each component of the XMVECTOR.
	// more efficient to keep everything SIMD until you are done with your calculations
	auto dotVector = DirectX::XMVector3Dot(g_vHalfVector, splatVector);


}

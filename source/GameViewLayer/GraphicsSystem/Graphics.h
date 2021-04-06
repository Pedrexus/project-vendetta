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

 Rendering Pipeline Stages:
	1. Input Assembler (IA)
		- reads geometric data (vertices and indices)
		- assemble gemometric primitives (triangles, lines)
		- vertices: 
			- spatial data + normal vectors + texture coordinates
			- vertex buffer: list of vertices - winding order
			* IASetPrimitiveTopology: sets the way the vertex buffer is read
		- indices: 
			- specify how the vertices are to be put together to form triangles
	2. Vertex Shader (VS)
		- draws vertices with the GPU
		- f: V -> V
		- Local space x World space
		- Camera:
			- View Space: Transformation from world coords to camera coords
			* XMMatrixLookAtLH: gives the direction the camera is looking at
			- Frustum: volume of space the camera sees
			- View coords: -1 < x/r < 1, -1 < y < 1, near < z < far
			- Normalized Depth Value: make the near and far planes as close as possible to minimize depth precision problems
			* XMMatrixPerspectiveFovLH
	3. Tesselation [optional]
		- interpolation of triangles to improve detail
		- benefits:
			1. saves memory: low-poly in memory -> high-poly on rendering
			2. animation and physics on low-poly
			3. level-of-detail (LOD): near -> tesselated, far -> not tesselated
	4. Geometry Shader (GS) [optional]
		- creates or destroys geometry
		- inputs primitives
	5. Clipping
		- discards geometry outside the frustum
		- clipps geometry intersecting the frustum's boundary 
		- performed by the hardware (Sutherland-Hodgeman clipping algorithm)
	6. Rasterization
		- computes pixel colors from the projected 3D triangles.
		-> TODO: read section
	7. Pixel Shader (PS)
		- runs on each pixel to compute color
		- executed on the GPU
		- used for: per-pixel lighting, reflections, shadowing
	8. Output Merger (OM)
		- writes the pixels to the back buffer
		- uses the depth/stencil buffer for filtering
		- may perform blending

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

	auto it = XMMatrixIdentity(); // 4x4 Identity matrix
}

// possible vertex definition
struct Vertex1
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex0;
	XMFLOAT2 Tex1;
};

D3D12_INPUT_ELEMENT_DESC vertex1Desc[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};
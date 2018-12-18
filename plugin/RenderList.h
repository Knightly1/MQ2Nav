//
// RenderList.h
//
// Implements a render list for rendering geometry

#pragma once

#include "Renderable.h"

#include <d3dx9.h>

#define GLM_FORCE_RADIANS
#include <glm.hpp>

#include <map>
#include <memory>
#include <vector>

//----------------------------------------------------------------------------

class RenderList : public Renderable
{
public:
	enum PrimitiveType
	{
		Prim_Points = 0,
		Prim_Lines,
		Prim_Triangles,
		Prim_Quads,

		Prim_Count
	};

	RenderList(IDirect3DDevice9* d3dDevice, PrimitiveType type);
	~RenderList();

	void Reset();

	//----------------------------------------------------------------------------
	// build the geometry

	void Begin(float size = 1.0f);

	void AddVertex(float x, float y, float z, unsigned int color, float u, float v);

	void End();

	//----------------------------------------------------------------------------

	// Render the geometry
	virtual void Render(RenderPhase phase);

	void SetTransform(D3DXMATRIX* mtx) { m_mtx = mtx; }
	void SetEQCoords(bool eqCoords) { m_eqCoords = eqCoords; }

	PrimitiveType GetType() const { return m_type; }

	void RenderDebugUI();

	// Release any resources we might be holding onto
	virtual void InvalidateDeviceObjects();

	// Create the resources we need to render
	virtual bool CreateDeviceObjects();

	// The number of primitives in the list
	int GetPrimitiveCount() const { return m_prims.size(); }

	// Set a range of primitives to draw when rendering. Clamped by the size of the list.
	void Debug_SetRenderRange(int begin, int end);
	void Debug_GetRenderRange(int& begin, int& end);

	void Debug_ShowTris();

private:
	void AddPoint();
	void AddLine();
	void AddTriangle();
	void AddQuad();

	// Create buffers if necessary
	void GenerateBuffers();

	struct Vertex
	{
		D3DXVECTOR3 pos;
		D3DCOLOR    col;
		D3DXVECTOR2 uv;
	};
	static const DWORD VertexType = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	PrimitiveType m_type;

	struct PrimitiveList
	{
		std::vector<uint32_t> indices;
		int vertices = 0;
		int count = 0;
		float size = 1.0;

		// used for rendering from the index buffer
		uint32_t startingIndex = 0;
	};

	uint32_t m_firstRender = 0;
	uint32_t m_lastRender = 0xffffffff;

	// buffer of temporary indices for the current primitive
	Vertex m_tempIndices[4];
	int m_tempIndex = 0;
	int m_tempMax = 0;

	D3DXMATRIX* m_mtx = nullptr;
	bool m_eqCoords = false;

private:
	IDirect3DDevice9* m_pDevice = nullptr;
	IDirect3DVertexBuffer9* m_pVB = nullptr;
	IDirect3DIndexBuffer9* m_pIB = nullptr;

	std::vector<Vertex> m_vertices;

	// keyed on thickness. We don't support thickness so it doesn't
	// make any difference at the moment.
	std::vector<std::unique_ptr<PrimitiveList>> m_prims;
	PrimitiveList* m_currentPrim;
};


class RenderGroup : public Renderable
{
public:
	RenderGroup(IDirect3DDevice9* device)
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
		{
			m_primLists[i].reset(new RenderList(device, static_cast<RenderList::PrimitiveType>(i)));
			m_primsEnabled[i] = true;
		}
	}

	~RenderGroup()
	{
		m_currentList = nullptr;
	}

	auto GetPrimsEnabled() -> auto& { return m_primsEnabled; }

	inline RenderList* GetRenderList(RenderList::PrimitiveType type)
	{
		assert(type >= 0 && type < RenderList::Prim_Count);
		return m_primLists[type].get();
	}

	inline void Reset()
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
			m_primLists[i]->Reset();

		m_currentList = nullptr;
	}

	inline void SetEQCoords(bool eqCoords)
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
			m_primLists[i]->SetEQCoords(eqCoords);
	}

	void SetTransform(D3DXMATRIX* mtx)
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
		{
			if (m_primsEnabled[i])
				m_primLists[i]->SetTransform(mtx);
		}
	}

	virtual void Render(Renderable::RenderPhase phase) override
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
		{
			if (m_primsEnabled[i])
				m_primLists[i]->Render(phase);
		}
	}

	virtual bool CreateDeviceObjects() override
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
			m_primLists[i]->CreateDeviceObjects();

		return true;
	}

	virtual void InvalidateDeviceObjects() override
	{
		for (int i = 0; i < RenderList::Prim_Count; ++i)
			m_primLists[i]->InvalidateDeviceObjects();
	}

	//----------------------------------------------------------------------------
	// build the geometry

	inline void Begin(RenderList::PrimitiveType type, float size = 1.0f)
	{
		m_currentList = m_primLists[type].get();
		m_currentList->Begin(size);
	}

	inline void AddVertex(float x, float y, float z, unsigned int color, float u, float v)
	{
		m_currentList->AddVertex(x, y, z, color, u, v);
	}

	void End()
	{
		m_currentList->End();
		m_currentList = nullptr;
	}

private:
	std::unique_ptr<RenderList> m_primLists[RenderList::Prim_Count];
	bool m_primsEnabled[RenderList::Prim_Count];

	RenderList* m_currentList = nullptr;
};

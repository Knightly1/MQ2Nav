//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#pragma once

#include "Recast.h"
#include "SampleInterfaces.h"

#include <glm/glm.hpp>

class BuildContext;

// Tool types.
enum SampleToolType
{
	TOOL_NONE = 0,
	TOOL_TILE_EDIT,
	TOOL_TILE_HIGHLIGHT,
	TOOL_TEMP_OBSTACLE,
	TOOL_NAVMESH_TESTER,
	TOOL_NAVMESH_PRUNE,
	TOOL_OFFMESH_CONNECTION,
	TOOL_CONVEX_VOLUME,
	TOOL_CROWD,
	MAX_TOOLS
};


enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

struct SampleTool
{
	virtual ~SampleTool() {}
	virtual int type() = 0;
	virtual void init(class Sample* sample) = 0;
	virtual void reset() = 0;
	virtual void handleMenu() = 0;
	virtual void handleClick(const glm::vec3& s, const glm::vec3& p, bool shift) = 0;
	virtual void handleRender() = 0;
	virtual void handleRenderOverlay(const glm::mat4& proj,
		const glm::mat4& model, const glm::ivec4& view) = 0;
	virtual void handleToggle() = 0;
	virtual void handleStep() = 0;
	virtual void handleUpdate(float dt) = 0;
};

struct SampleToolState {
	virtual ~SampleToolState() {}
	virtual void init(class Sample* sample) = 0;
	virtual void reset() = 0;
	virtual void handleRender() = 0;
	virtual void handleRenderOverlay(const glm::mat4& proj,
		const glm::mat4& model, const glm::ivec4& view) = 0;
	virtual void handleUpdate(const float dt) = 0;
};

class Sample
{
protected:
	class InputGeom* m_geom;
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;

	unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
	int m_partitionType;

	SampleTool* m_tool;
	SampleToolState* m_toolStates[MAX_TOOLS];

	BuildContext* m_ctx;

public:
	Sample();
	virtual ~Sample();

	void setContext(BuildContext* ctx) { m_ctx = ctx; }

	void setTool(SampleTool* tool);
	SampleToolState* getToolState(int type) { return m_toolStates[type]; }
	void setToolState(int type, SampleToolState* s) { m_toolStates[type] = s; }

	virtual void handleSettings();
	virtual void handleTools();
	virtual void handleDebugMode();
	virtual void handleClick(const glm::vec3& s, const glm::vec3& p, bool shift);
	virtual void handleToggle();
	virtual void handleStep();
	virtual void handleRender();
	virtual void handleRenderOverlay(const glm::mat4& proj, const glm::mat4& model, const glm::ivec4& view);
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild();
	virtual void handleUpdate(float dt);

	virtual class InputGeom* getInputGeom() { return m_geom; }
	virtual class dtNavMesh* getNavMesh() { return m_navMesh; }
	virtual class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
	virtual class dtCrowd* getCrowd() { return m_crowd; }
	virtual float getAgentRadius() { return m_agentRadius; }
	virtual float getAgentHeight() { return m_agentHeight; }
	virtual float getAgentClimb() { return m_agentMaxClimb; }
	virtual const float* getBoundsMin();
	virtual const float* getBoundsMax();

	inline unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
	inline void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }

	void updateToolStates(float dt);
	void initToolStates(Sample* sample);
	void resetToolStates();
	void renderToolStates();
	void renderOverlayToolStates(const glm::mat4& proj, const glm::mat4& model, const glm::ivec4& view);

	void resetCommonSettings();
	void handleCommonSettings();
};

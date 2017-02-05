﻿//
// ConvexVolumeTool.cpp
//
#include "ConvexVolumeTool.h"
#include "InputGeom.h"
#include "NavMeshTool.h"
#include "common/Utilities.h"

#include <Recast.h>
#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>

#include <imgui/imgui.h>
#include <imgui/imgui_custom/imgui_user.h>
#include <glm/gtc/type_ptr.hpp>

// Quick and dirty convex hull.

// Calculates convex hull on xz-plane of points on 'pts',
// stores the indices of the resulting hull in 'out' and
// returns number of points on hull.
static void convexhull(const std::vector<glm::vec3>& pts, std::vector<uint32_t>& out)
{
	// Find lower-leftmost point.
	uint32_t hull = 0;
	for (uint32_t i = 1; i < static_cast<uint32_t>(pts.size()); ++i)
		if (cmppt(pts[i], pts[hull]))
			hull = i;
	// Gift wrap hull.
	uint32_t endpt = 0;
	out.clear();
	do
	{
		out.push_back(hull);
		endpt = 0;
		for (uint32_t j = 1; j < static_cast<uint32_t>(pts.size()); ++j)
			if (hull == endpt || left(pts[hull], pts[endpt], pts[j]))
				endpt = j;
		hull = endpt;
	} while (endpt != out[0]);
}

static int pointInPoly(int nvert, const glm::vec3* verts, const glm::vec3& p)
{
	bool c = false;

	for (int i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		const glm::vec3& vi = verts[i];
		const glm::vec3& vj = verts[j];

		if (((vi[2] > p[2]) != (vj[2] > p[2])) &&
			(p[0] < (vj[0] - vi[0]) * (p[2] - vi[2]) / (vj[2] - vi[2]) + vi[0]))
		{
			c = !c;
		}
	}
	return c;
}

//----------------------------------------------------------------------------

ConvexVolumeTool::ConvexVolumeTool()
{
}

ConvexVolumeTool::~ConvexVolumeTool()
{
}

void ConvexVolumeTool::init(NavMeshTool* meshTool)
{
	m_meshTool = meshTool;
}

void ConvexVolumeTool::reset()
{
	m_pts.clear();
	m_hull.clear();
}

void ConvexVolumeTool::handleMenu()
{
	ImGui::SliderFloat("Shape Height", &m_boxHeight, 0.1f, 20.0f);
	ImGui::SliderFloat("Shape Descent", &m_boxDescent, 0.1f, 20.0f);
	ImGui::SliderFloat("Poly Offset", &m_polyOffset, 0.0f, 10.0f);

	ImGui::Separator();

	ImGui::Text("Area Type");

	ImGui::Indent();

	if (ImGui::RadioButton("Ground", m_areaType == PolyArea::Ground))
		m_areaType = PolyArea::Ground;
	if (ImGui::RadioButton("Unwalkable", m_areaType == PolyArea::Unwalkable))
		m_areaType = PolyArea::Unwalkable;
#if 0
	if (ImGui::RadioButton("Grass", m_areaType == PolyArea::Grass))
		m_areaType = PolyArea::Grass;
	if (ImGui::RadioButton("Road", m_areaType == PolyArea::Road))
		m_areaType = PolyArea::Road;
	if (ImGui::RadioButton("Water", m_areaType == PolyArea::Water))
		m_areaType = PolyArea::Water;
	if (ImGui::RadioButton("Door", m_areaType == PolyArea::Door))
		m_areaType = PolyArea::Door;
#endif
	ImGui::Unindent();

	ImGui::Separator();

	if (ImGui::Button("Clear Shape"))
	{
		reset();
	}
}

void ConvexVolumeTool::handleClick(const glm::vec3& /*s*/, const glm::vec3& p, bool shift)
{
	if (!m_meshTool) return;
	InputGeom* geom = m_meshTool->getInputGeom();
	if (!geom) return;

	if (shift)
	{
		// Delete
		size_t nearestIndex = -1;
		for (size_t i = 0; i < geom->getConvexVolumeCount(); ++i)
		{
			const ConvexVolume* vol = geom->getConvexVolume(i);

			if (pointInPoly(static_cast<int>(vol->verts.size()), &vol->verts[0], p)
				&& p.y >= vol->hmin && p.y <= vol->hmax)
			{
				nearestIndex = i;
			}
		}

		// If end point close enough, delete it.
		if (nearestIndex != -1)
		{
			geom->deleteConvexVolume(nearestIndex);
		}
	}
	else
	{
		// Create
		bool alt = (SDL_GetModState() & KMOD_ALT) != 0;

		// If clicked on that last pt, create the shape.
		if (m_pts.size() > 0 && (alt || distSqr(p, m_pts[m_pts.size() - 1]) < rcSqr(0.2f)))
		{
			if (m_hull.size() > 2)
			{
				std::vector<glm::vec3> verts(m_hull.size());

				// Create shape.
				for (size_t i = 0; i < m_hull.size(); ++i)
					verts[i] = m_pts[m_hull[i]];

				float minh = FLT_MAX, maxh = 0;
				for (size_t i = 0; i < m_hull.size(); ++i)
					minh = glm::min(minh, verts[i].y);

				minh -= m_boxDescent;
				maxh = minh + m_boxHeight;

				if (m_polyOffset > 0.01f)
				{
					std::vector<glm::vec3> offset(m_hull.size() * 2);

					int noffset = rcOffsetPoly(glm::value_ptr(verts[0]), static_cast<int>(verts.size()),
						m_polyOffset, glm::value_ptr(offset[0]), static_cast<int>(verts.size() * 2));
					if (noffset > 0)
					{
						geom->addConvexVolume(offset, minh, maxh, m_areaType);
					}
				}
				else
				{
					geom->addConvexVolume(verts, minh, maxh, m_areaType);
				}
			}

			reset();
		}
		else
		{
			// Add new point
			m_pts.push_back(p);

			// Update hull.
			if (m_pts.size() >= 2)
				convexhull(m_pts, m_hull);
			else
				m_hull.clear();
		}
	}
}

void ConvexVolumeTool::handleRender()
{
	DebugDrawGL dd;

	// Find height extents of the shape.
	float minh = FLT_MAX, maxh = 0;
	for (size_t i = 0; i < m_pts.size(); ++i)
		minh = glm::min(minh, m_pts[i].y);
	minh -= m_boxDescent;
	maxh = minh + m_boxHeight;

	dd.begin(DU_DRAW_POINTS, 4.0f);
	for (size_t i = 0; i < m_pts.size(); ++i)
	{
		unsigned int col = duRGBA(255, 255, 255, 255);
		if (i == m_pts.size() - 1)
			col = duRGBA(240, 32, 16, 255);
		dd.vertex(m_pts[i].x, m_pts[i].y + 0.1f, m_pts[i].z, col);
	}
	dd.end();

	dd.begin(DU_DRAW_LINES, 2.0f);
	for (size_t i = 0, j = m_hull.size() - 1; i < m_hull.size(); j = i++)
	{
		const glm::vec3& vi = m_pts[m_hull[j]];
		const glm::vec3& vj = m_pts[m_hull[i]];
		dd.vertex(vj.x, minh, vj.z, duRGBA(255, 255, 255, 64));
		dd.vertex(vi.x, minh, vi.z, duRGBA(255, 255, 255, 64));
		dd.vertex(vj.x, maxh, vj.z, duRGBA(255, 255, 255, 64));
		dd.vertex(vi.x, maxh, vi.z, duRGBA(255, 255, 255, 64));
		dd.vertex(vj.x, minh, vj.z, duRGBA(255, 255, 255, 64));
		dd.vertex(vj.x, maxh, vj.z, duRGBA(255, 255, 255, 64));
	}
	dd.end();
}

void ConvexVolumeTool::handleRenderOverlay(const glm::mat4& /*proj*/,
	const glm::mat4& /*model*/, const glm::ivec4& view)
{
	// Tool help
	if (m_pts.empty())
	{
		ImGui::RenderTextRight(-330, -(view[3] - 40), ImVec4(255, 255, 255, 192),
			"LMB: Create new shape.  SHIFT+LMB: Delete existing shape (click inside a shape).");
	}
	else
	{
		ImGui::RenderTextRight(-330, -(view[3] - 40), ImVec4(255, 255, 255, 192),
			"Click LMB to add new points. Alt+Click to finish the shape.");
		ImGui::RenderTextRight(-330, -(view[3] - 60), ImVec4(255, 255, 255, 192),
			"The shape will be convex hull of all added points.");
	}
}

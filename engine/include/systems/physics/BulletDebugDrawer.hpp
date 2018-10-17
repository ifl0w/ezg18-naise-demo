#pragma once

#include <LinearMath/btIDebugDraw.h>

#include <memory>
#include <unordered_map>

#include "BulletDebugDrawerMesh.hpp"

#include <scene/Entity.hpp>
#include <components/MaterialComponent.hpp>
#include <components/MeshComponent.hpp>

namespace NAISE {
namespace Engine {

struct PhysicsDebugComponent: public Component {
  glm::vec3 color = glm::vec3(1,0,0);
  BulletDebugDrawerMesh mesh;
};

class BulletDebugDrawer
		: public btIDebugDraw {
public:
	BulletDebugDrawer();

	/*
	 * Collecting all lines that come from bullet and batch them together.
	 */
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
						  const btVector3& color) override;

	void reportErrorWarning(const char* warningString) override;

	void draw3dText(const btVector3& location, const char* textString) override;

	void setDebugMode(int debugMode) override;

	int getDebugMode() const override { return m_debugMode; }

	void beginDebugFrame();
	void finishMeshes();
	void enableDebugging();
	void disableDebugging();

protected:
	int m_debugMode = 0;

	std::vector<std::shared_ptr<Entity>> debugEntities;
};

}
}

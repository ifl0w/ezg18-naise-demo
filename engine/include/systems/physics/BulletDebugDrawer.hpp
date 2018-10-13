#pragma once

#define BT_USE_DOUBLE_PRECISION

#include <LinearMath/btIDebugDraw.h>

#include "../../materials/shaders/Shader.h"
#include "../../materials/PhongMaterial.h"
#include "../../meshes/Mesh.h"
#include "BulletDebugDrawerMesh.h"

namespace NAISE {
namespace ENGINE {

class Object; // forward declaration

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

	std::shared_ptr<Object> retrieveDebugObject();

protected:
	int m_debugMode = 0;
	std::shared_ptr<Material> material;
	std::shared_ptr<BulletDebugDrawerMesh> currentMesh;
};

}
}

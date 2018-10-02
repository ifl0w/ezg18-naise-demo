
#pragma once

#include <glbinding/gl/gl.h>

using namespace gl;

namespace NAISE {
namespace Engine {

class PingPongTarget {
public:
	PingPongTarget();
	virtual ~PingPongTarget();

	virtual void use() = 0;

protected:
	GLuint fboPingPong;

};

}
}

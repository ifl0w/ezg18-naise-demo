#pragma once

/*
	Drawable Objects implement the draw function that is used to render them in the render loop.
*/

namespace NAISE {
namespace Engine {

class Drawable {
public:
	virtual void draw() = 0;
};

}
}
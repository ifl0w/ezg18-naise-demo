#include <systems/debugging/DebugSystem.hpp>
#include <Engine.hpp>

#include "imgui.h"
#include "systems/debugging/imgui-impl/imgui_impl_sdl.hpp"
#include "systems/debugging/imgui-impl/imgui_impl_opengl3.hpp"

using namespace NAISE::Engine;

DebugSystem::DebugSystem() {

	Engine::getEventManager().event<DebugEvents::ToggleInterface>().subscribe([&](){
	  _interfaceEnabled = !_interfaceEnabled;
	  NAISE_INFO_CONSOL("Toggle debug interface.");
	});

}

void DebugSystem::process(microseconds deltaTime) {
	std::chrono::duration<float, std::milli> msf = deltaTime;
	std::chrono::duration<float> sec = deltaTime;

	frameTimes.erase(frameTimes.begin());
	frameTimes.push_back(msf.count());

	if(_interfaceEnabled) {
		ImGui::Text("Delta time: %f", msf.count());
		ImGui::Text("FPS: %f", 1/sec.count());


		// Plot some values
		const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		ImGui::PlotLines("Frame Times", frameTimes.data(), static_cast<int>(frameTimes.size()));
	}
}

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
	if(_interfaceEnabled) {
		renderProfilingInterface(deltaTime);
	}
}

void DebugSystem::renderProfilingInterface(microseconds deltaTime) {
	std::chrono::duration<float, std::milli> msf = deltaTime;
	std::chrono::duration<float> sec = deltaTime;

	ImGui::Text("Delta time: %f", msf.count());
	ImGui::Text("FPS: %f", 1/sec.count());

	accumFrameTime += msf.count();
	frameTimeSamples++;
	if (accumFrameTime >= minimumPlotFrameTime) {
		frameTimes.erase(frameTimes.begin());
		frameTimes.push_back(accumFrameTime/frameTimeSamples);

		frameTimeSamples = 0;
		accumFrameTime = 0;
	}

	// Plot some values
	ImGui::PlotLines("Frame Times", frameTimes.data(), static_cast<int>(frameTimes.size()));

	ImGui::TextColored(ImVec4(1,1,0,1), "System times:");
	ImGui::BeginChild("Scrolling");
	for (const auto& elem: Engine::getSystemsManager().getSystemTimes()) {
		ImGui::Text("%s: %f", elem.first.c_str(), elem.second);
	}
	ImGui::EndChild();
}

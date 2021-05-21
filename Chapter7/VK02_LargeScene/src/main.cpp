#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/MultiRenderer.h"

struct MyApp: public CameraApp
{
	MyApp()
	: CameraApp(-95, -95)
	, envMap(ctx_.resources.loadCubeMap("data/piazza_bologni_1k.hdr"))
	, irrMap(ctx_.resources.loadCubeMap("data/piazza_bologni_1k_irradiance.hdr"))
	, sceneData(ctx_, "data/meshes/test.meshes", "data/meshes/test.scene", "data/meshes/test.materials", envMap, irrMap)
	, sceneData2(ctx_, "data/meshes/test2.meshes", "data/meshes/test2.scene", "data/meshes/test2.materials", envMap, irrMap)
	, multiRenderer(ctx_, sceneData)
	, multiRenderer2(ctx_, sceneData2)
	, imgui(ctx_)
	{
		positioner = CameraPositioner_FirstPerson(glm::vec3(-10.0f, -3.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

		onScreenRenderers_.emplace_back(multiRenderer);
		onScreenRenderers_.emplace_back(multiRenderer2);
		onScreenRenderers_.emplace_back(imgui, false);
	}

	void draw3D() override {
		const mat4 p = getDefaultProjection();
		const mat4 view =camera.getViewMatrix();

		multiRenderer.setMatrices(p, view);
		multiRenderer2.setMatrices(p, view);

		multiRenderer.setCameraPosition(positioner.getPosition());
		multiRenderer2.setCameraPosition(positioner.getPosition());
	}
private:
	VulkanTexture envMap;
	VulkanTexture irrMap;

	VKSceneData sceneData;
	VKSceneData sceneData2;

	MultiRenderer multiRenderer;
	MultiRenderer multiRenderer2;
	GuiRenderer imgui;
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}

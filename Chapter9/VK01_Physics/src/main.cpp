#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/MultiRenderer.h"
#include "shared/vkFramework/InfinitePlaneRenderer.h"

#include "physics.h"

struct MyApp: public CameraApp
{
	MyApp()
	: CameraApp(-90, -90)
	, plane(ctx_)
	, sceneData(ctx_, "data/meshes/cube.meshes", "data/meshes/cube.scene", "data/meshes/cube.material", {}, {})
	, multiRenderer(ctx_, sceneData, "data/shaders/chapter09/VK01_Simple.vert", "data/shaders/chapter09/VK01_Simple.frag")
	, imgui(ctx_)
	{
		positioner = CameraPositioner_FirstPerson(glm::vec3(0.0f, 50.0f, 100.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

		onScreenRenderers_.emplace_back(plane, false);
		onScreenRenderers_.emplace_back(multiRenderer);
		onScreenRenderers_.emplace_back(imgui, false);

		const int maxCubes = 100;

		for (int i = 0 ; i < maxCubes ; i++)
			physics.addBox(vec3(1), btQuaternion(0,0,0,1), vec3(0.f, 2.f + 3.f * i, 0.f), 3.f);
	}

	void draw3D() override
	{
		const mat4 p = getDefaultProjection();
		const mat4 view = camera.getViewMatrix();

		multiRenderer.setMatrices(p, view);
		multiRenderer.setCameraPosition(positioner.getPosition());

		plane.setMatrices(p, view, glm::mat4(1.f));

		sceneData.scene_.globalTransform_[0] = glm::mat4(1.f);
		for (size_t i = 0; i < physics.boxTransform.size(); i++)
			sceneData.scene_.globalTransform_[i] = physics.boxTransform[i];
	}

	void update(float deltaSeconds) override
	{
		CameraApp::update(deltaSeconds);

		physics.update(deltaSeconds);

		sceneData.uploadGlobalTransforms();
	}
private:
	InfinitePlaneRenderer plane;

	VKSceneData sceneData;
	MultiRenderer multiRenderer;

	GuiRenderer imgui;

	Physics physics;
};

void generateMeshFile()
{
	constexpr uint32_t cubeVtxCount = 8;
	constexpr uint32_t cubeIdxCount = 36;

	Mesh cubeMesh { .lodCount = 1, .streamCount = 1, .lodOffset = { 0, cubeIdxCount }, .streamOffset = { 0 } };

	MeshData md = {
		.indexData_ = std::vector<uint32_t>(cubeIdxCount, 0),
		.vertexData_ = std::vector<float> (cubeVtxCount * 8, 0),
		.meshes_ = std::vector<Mesh> { cubeMesh },
		.boxes_ = std::vector<BoundingBox>(1)
	};

	saveMeshData("data/meshes/cube.meshes", md);
}

void generateData()
{
	const int numCubes = 100;

	Scene cubeScene;
	addNode(cubeScene, -1, 0);
	for (int i = 0 ; i < numCubes ; i++)
	{
		addNode(cubeScene, 0, 1);
		cubeScene.meshes_[i + 1] = 0;
		cubeScene.materialForNode_[i + 1] = 0;
	}

	saveScene("data/meshes/cube.scene", cubeScene);

	std::vector<std::string> files = { "data/ch2_sample3_STB.jpg" };
	std::vector<MaterialDescription> materials = { { .albedoColor_ = gpuvec4(1,0,0,1), .albedoMap_ = 0xFFFFFFF, .normalMap_ = 0xFFFFFFFF } };
	saveMaterials("data/meshes/cube.material", materials, files);
}

int main()
{
	generateMeshFile();
	generateData();

	MyApp app;
	app.mainLoop();
	return 0;
}

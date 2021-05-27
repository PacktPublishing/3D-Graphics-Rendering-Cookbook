#include "shared/vkFramework/VulkanApp.h"
#include "shared/vkFramework/LineCanvas.h"
#include "shared/vkFramework/QuadRenderer.h"
#include "shared/vkFramework/GuiRenderer.h"
#include "shared/vkFramework/VulkanShaderProcessor.h"
#include "shared/vkFramework/InfinitePlaneRenderer.h"

bool g_RotateModel = true;

float g_ModelAngle = 0.0f;

float g_LightAngle = 60.0f;
float g_LightNear = 1.0f;
float g_LightFar = 100.0f;

float g_LightDist = 50.0f;
float g_LightXAngle = -0.5f;
float g_LightYAngle = 0.55f;

const char* g_meshFile = "data/rubber_duck/scene.gltf";
const float g_meshScale = 20.0f; // scale from shader

struct Uniforms
{
	mat4 mvp;
	mat4 model;
	mat4 shadowMatrix;
	vec4 cameraPos;	
	vec4 lightPos;
	float meshScale;
	float effects;
};

static_assert(sizeof(Uniforms) == sizeof(float) * 58);

struct MyApp: public CameraApp
{
	MyApp(): CameraApp(-95, -95),

		meshBuffer(ctx_.resources.loadMeshToBuffer(g_meshFile, true, true, meshVertices, meshIndices)),
		planeBuffer(ctx_.resources.createPlaneBuffer_XY(2.0f, 2.0f)),

		meshUniformBuffer(ctx_.resources.addUniformBuffer(sizeof(Uniforms))),
		shadowUniformBuffer(ctx_.resources.addUniformBuffer(sizeof(Uniforms))),

		meshDepth(ctx_.resources.addDepthTexture()),
		meshColor(ctx_.resources.addColorTexture()),

		meshShadowDepth(ctx_.resources.addDepthTexture()),
		meshShadowColor(ctx_.resources.addColorTexture()),

		meshRenderer(ctx_, meshUniformBuffer, meshBuffer,
			{
				fsTextureAttachment(meshShadowDepth),
				fsTextureAttachment(ctx_.resources.loadTexture2D("data/rubber_duck/textures/Duck_baseColor.png")),
			},
			{ meshColor, meshDepth }, { "data/shaders/chapter08/VK01_scene.vert", "data/shaders/chapter08/VK01_scene.frag" }),

		depthRenderer(ctx_, shadowUniformBuffer, meshBuffer, { },
			{ meshShadowColor, meshShadowDepth }, { "data/shaders/chapter08/VK01_shadow.vert", "data/shaders/chapter08/VK01_shadow.frag" }, true),

		planeRenderer(ctx_, meshUniformBuffer, planeBuffer,
			{
				fsTextureAttachment(meshShadowDepth),
				fsTextureAttachment(ctx_.resources.loadTexture2D("data/ch2_sample3_STB.jpg"))
			},
			{ meshColor, meshDepth }, { "data/shaders/chapter08/VK01_scene.vert", "data/shaders/chapter08/VK01_scene.frag" }),

		canvas(ctx_, { meshColor, meshDepth }, true),

		quads(ctx_, { meshColor, meshDepth, meshShadowColor, meshShadowDepth }),
		imgui(ctx_, { meshShadowColor, meshShadowDepth } )
	{
		onScreenRenderers_.emplace_back(canvas);
		onScreenRenderers_.emplace_back(meshRenderer);
		onScreenRenderers_.emplace_back(depthRenderer);
		onScreenRenderers_.emplace_back(planeRenderer);

		onScreenRenderers_.emplace_back(quads, false);
		onScreenRenderers_.emplace_back(imgui, false);

		positioner.lookAt(glm::vec3(-85.0f, 85.0f, 85.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

		printf("Verts: %d\n", (int)meshVertices.size() / 8);

		canvas.clear();
	}

	void update(float deltaSeconds) override
	{
		if (g_RotateModel)
			g_ModelAngle += deltaSeconds;

		while (g_ModelAngle > 360.0f)
			g_ModelAngle -= 360.0f;
	}

	void drawUI() override
	{
		ImGui::Begin("Control", nullptr);
		ImGui::Checkbox("Rotate model", &g_RotateModel);
		ImGui::Text("Light parameters", nullptr);
		ImGui::SliderFloat("Proj::Angle", &g_LightAngle, 10.f, 170.0f);
		ImGui::SliderFloat("Proj::Near", &g_LightNear, 0.1f, 5.0f);
		ImGui::SliderFloat("Proj::Far",  &g_LightFar,  0.1f, 100.0f);
		ImGui::SliderFloat("Pos::Dist",    &g_LightDist,     0.5f, 100.0f);
		ImGui::SliderFloat("Pos::AngleX",  &g_LightXAngle,  -3.15f, +3.15f);
		ImGui::SliderFloat("Pos::AngleY",  &g_LightYAngle,  -3.15f, +3.15f);
		ImGui::End();

		imguiTextureWindow("Color", 1);
		imguiTextureWindow("Depth", 2 | (0x1 << 16));
	}

	void draw3D() override
	{
		const mat4 proj = getDefaultProjection();
		const mat4 view = glm::scale(mat4(1.f), vec3(1.f, -1.f, 1.f)) * camera.getViewMatrix();

		const mat4 m1 = glm::rotate(
			glm::translate(mat4(1.0f), vec3(0.f, 0.5f, -1.5f)) * glm::rotate(mat4(1.f), -0.5f * glm::pi<float>(), vec3(1, 0, 0)),
			g_ModelAngle, vec3(0.0f, 0.0f, 1.0f));

		canvas.setCameraMatrix(proj * view);

		quads.clear();
		quads.quad(-1.0f, -1.0f, 1.0f, 1.0f, 0);
		canvas.clear();

		const glm::mat4 rotY = glm::rotate(mat4(1.f), g_LightYAngle, glm::vec3(0, 1, 0));
		const glm::mat4 rotX = glm::rotate(rotY, g_LightXAngle, glm::vec3(1, 0, 0));
		const glm::vec4 lightPos = rotX * glm::vec4(0, 0, g_LightDist, 1.0f);

		const mat4 lightProj = glm::perspective(glm::radians(g_LightAngle), 1.0f, g_LightNear, g_LightFar);
		const mat4 lightView = glm::lookAt(glm::vec3(lightPos), vec3(0), vec3(0, 1, 0));

		renderCameraFrustum(canvas, lightView, lightProj, vec4(0.0f, 1.0f, 0.0f, 1.0f));

		const Uniforms uniDepth = {
			.mvp = lightProj * lightView * m1,
			.model = m1,
			.shadowMatrix = mat4(1.0f),
			.cameraPos = vec4(camera.getPosition(), 1.0f),
			.lightPos = lightPos,
			.meshScale = g_meshScale,
		};
		uploadBufferData(ctx_.vkDev, shadowUniformBuffer.memory, 0, &uniDepth, sizeof(uniDepth));

		const Uniforms uni = {
			.mvp = proj * view * m1,
			.model = m1,
			.shadowMatrix = lightProj * lightView,
			.cameraPos = vec4(camera.getPosition(), 1.0f),
			.lightPos = lightPos,
			.meshScale = g_meshScale,
		};
		uploadBufferData(ctx_.vkDev, meshUniformBuffer.memory, 0, &uni, sizeof(uni));
	}

private:
	std::vector<float> meshVertices;
	std::vector<unsigned int> meshIndices;

	std::pair<BufferAttachment, BufferAttachment> meshBuffer;
	std::pair<BufferAttachment, BufferAttachment> planeBuffer;

	VulkanBuffer meshUniformBuffer;
	VulkanBuffer shadowUniformBuffer;
	
	VulkanTexture meshDepth, meshColor;
	VulkanTexture meshShadowDepth, meshShadowColor;

	OffscreenMeshRenderer meshRenderer;
	OffscreenMeshRenderer depthRenderer;
	OffscreenMeshRenderer planeRenderer;

	LineCanvas canvas;

	QuadRenderer quads;
	GuiRenderer imgui;
};

int main()
{
	MyApp app;
	app.mainLoop();
	return 0;
}

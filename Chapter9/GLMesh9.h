#pragma once

#include <functional>

const GLuint kBufferIndex_PerFrameUniforms = 0;
const GLuint kBufferIndex_ModelMatrices = 1;
const GLuint kBufferIndex_Materials = 2;

struct DrawElementsIndirectCommand
{
	GLuint count_;
	GLuint instanceCount_;
	GLuint firstIndex_;
	GLuint baseVertex_;
	GLuint baseInstance_;
};

class GLIndirectBuffer final
{
public:
	explicit GLIndirectBuffer(size_t maxDrawCommands)
	: bufferIndirect_(sizeof(DrawElementsIndirectCommand) * maxDrawCommands, nullptr, GL_DYNAMIC_STORAGE_BIT)
	, drawCommands_(maxDrawCommands)
	{}

	GLuint getHandle() const { return bufferIndirect_.getHandle(); }
	void uploadIndirectBuffer()
	{
		glNamedBufferSubData(bufferIndirect_.getHandle(), 0, sizeof(DrawElementsIndirectCommand) * drawCommands_.size(), drawCommands_.data());
	}

	void selectTo(GLIndirectBuffer& buf, const std::function<bool(const DrawElementsIndirectCommand&)>& pred)
	{
		buf.drawCommands_.clear();
		for (const auto& c : drawCommands_)
		{
			if (pred(c))
				buf.drawCommands_.push_back(c);
		}
		buf.uploadIndirectBuffer();
	}

	std::vector<DrawElementsIndirectCommand> drawCommands_;

private:
	GLBuffer bufferIndirect_;
};

template <typename GLSceneDataType>
class GLMesh final
{
public:
	explicit GLMesh(const GLSceneDataType& data)
		: numIndices_(data.header_.indexDataSize / sizeof(uint32_t))
		, bufferIndices_(data.header_.indexDataSize, data.meshData_.indexData_.data(), 0)
		, bufferVertices_(data.header_.vertexDataSize, data.meshData_.vertexData_.data(), 0)
		, bufferMaterials_(sizeof(MaterialDescription) * data.materials_.size(), data.materials_.data(), GL_DYNAMIC_STORAGE_BIT)
		, bufferModelMatrices_(sizeof(glm::mat4) * data.shapes_.size(), nullptr, GL_DYNAMIC_STORAGE_BIT)
		, bufferIndirect_(data.shapes_.size())
	{
		glCreateVertexArrays(1, &vao_);
		glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());
		glVertexArrayVertexBuffer(vao_, 0, bufferVertices_.getHandle(), 0, sizeof(vec3) + sizeof(vec3) + sizeof(vec2));
		// position
		glEnableVertexArrayAttrib(vao_, 0);
		glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao_, 0, 0);
		// uv
		glEnableVertexArrayAttrib(vao_, 1);
		glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
		glVertexArrayAttribBinding(vao_, 1, 0);
		// normal
		glEnableVertexArrayAttrib(vao_, 2);
		glVertexArrayAttribFormat(vao_, 2, 3, GL_FLOAT, GL_TRUE, sizeof(vec3) + sizeof(vec2));
		glVertexArrayAttribBinding(vao_, 2, 0);

		std::vector<glm::mat4> matrices(data.shapes_.size());

		// prepare indirect commands buffer
		for (size_t i = 0; i != data.shapes_.size(); i++)
		{
			const uint32_t meshIdx = data.shapes_[i].meshIndex;
			const uint32_t lod = data.shapes_[i].LOD;
			bufferIndirect_.drawCommands_[i] = {
				.count_ = data.meshData_.meshes_[meshIdx].getLODIndicesCount(lod),
				.instanceCount_ = 1,
				.firstIndex_ = data.shapes_[i].indexOffset,
				.baseVertex_ = data.shapes_[i].vertexOffset,
				.baseInstance_ = data.shapes_[i].materialIndex + (uint32_t(i) << 16)
			};
			matrices[i] = data.scene_.globalTransform_[data.shapes_[i].transformIndex];
		}

		bufferIndirect_.uploadIndirectBuffer();

		glNamedBufferSubData(bufferModelMatrices_.getHandle(), 0, matrices.size() * sizeof(mat4), matrices.data());
	}

	void updateMaterialsBuffer(const GLSceneDataType& data)
	{
		glNamedBufferSubData(bufferMaterials_.getHandle(), 0, sizeof(MaterialDescription) * data.materials_.size(), data.materials_.data());
	}

	void draw(size_t numDrawCommands, const GLIndirectBuffer* buffer = nullptr) const
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_Materials, bufferMaterials_.getHandle());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_ModelMatrices, bufferModelMatrices_.getHandle());
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, (buffer ? *buffer : bufferIndirect_).getHandle());
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, (GLsizei)numDrawCommands, 0);
	}

	~GLMesh()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	GLMesh(const GLMesh&) = delete;
	GLMesh(GLMesh&&) = default;

//private:
	GLuint vao_;
	uint32_t numIndices_;

	GLBuffer bufferIndices_;
	GLBuffer bufferVertices_;
	GLBuffer bufferMaterials_;
	GLBuffer bufferModelMatrices_;

	GLIndirectBuffer bufferIndirect_;
};

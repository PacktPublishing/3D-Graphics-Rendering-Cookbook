#include "shared/scene/Material.h"
#include "shared/scene/MergeUtil.h"

#include <map>

static uint32_t shiftMeshIndices(MeshData& meshData, const std::vector<uint32_t>& meshesToMerge)
{
	auto minVtxOffset = std::numeric_limits<uint32_t>::max();
	for (auto i: meshesToMerge)
		minVtxOffset = std::min(meshData.meshes_[i].vertexOffset, minVtxOffset);

	auto mergeCount = 0u; // calculated by summing index counts in meshesToMerge

	// now shift all the indices in individual index blocks [use minVtxOffset]
	for (auto i: meshesToMerge)
	{
		auto& m = meshData.meshes_[i];
		// for how much should we shift the indices in mesh [m]
		const uint32_t delta = m.vertexOffset - minVtxOffset;

		const auto idxCount = m.getLODIndicesCount(0);
		for (auto ii = 0u ; ii < idxCount ; ii++)
			meshData.indexData_[m.indexOffset + ii] += delta;

		m.vertexOffset = minVtxOffset;

		// sum all the deleted meshes' indices
		mergeCount += idxCount;
	}

	return meshData.indexData_.size() - mergeCount;
}

// All the meshesToMerge now have the same vertexOffset and individual index values are shifted by appropriate amount
// Here we move all the indices to appropriate places in the new index array
static void mergeIndexArray(MeshData& md, const std::vector<uint32_t>& meshesToMerge, std::map<uint32_t, uint32_t>& oldToNew)
{
	std::vector<uint32_t> newIndices(md.indexData_.size());
	// Two offsets in the new indices array (one begins at the start, the second one after all the copied indices)
	uint32_t copyOffset = 0,
	         mergeOffset = shiftMeshIndices(md, meshesToMerge);

	const auto mergedMeshIndex = md.meshes_.size() - meshesToMerge.size();
	auto newIndex = 0u;
	for (auto midx = 0u ; midx < md.meshes_.size() ; midx++)
	{
		const bool shouldMerge = std::binary_search( meshesToMerge.begin(), meshesToMerge.end(), midx);

		oldToNew[midx] = shouldMerge ? mergedMeshIndex : newIndex;
		newIndex += shouldMerge ? 0 : 1;

		auto& mesh = md.meshes_[midx];
		auto idxCount = mesh.getLODIndicesCount(0);
		// move all indices to the new array at mergeOffset
		const auto start = md.indexData_.begin() + mesh.indexOffset;
		mesh.indexOffset = copyOffset;
		const auto offsetPtr = shouldMerge ? &mergeOffset : &copyOffset;
		std::copy(start, start + idxCount, newIndices.begin() + *offsetPtr);
		*offsetPtr += idxCount;
	}

	md.indexData_ = newIndices;

	// all the merged indices are now in lastMesh
	Mesh lastMesh = md.meshes_[meshesToMerge[0]];
	lastMesh.indexOffset = copyOffset;
	lastMesh.lodOffset[0] = copyOffset;
	lastMesh.lodOffset[1] = mergeOffset;
	lastMesh.lodCount = 1;
	md.meshes_.push_back(lastMesh);
}

void mergeScene(Scene& scene, MeshData& meshData, const std::string& materialName)
{
	// Find material index
	int oldMaterial = (int)std::distance(std::begin(scene.materialNames_), std::find(std::begin(scene.materialNames_), std::end(scene.materialNames_), materialName));

	std::vector<uint32_t> toDelete;

	for (auto i = 0u ; i < scene.hierarchy_.size() ; i++)
		if (scene.meshes_.contains(i) && scene.materialForNode_.contains(i) && (scene.materialForNode_.at(i) == oldMaterial))
			toDelete.push_back(i);

	std::vector<uint32_t> meshesToMerge(toDelete.size());

	// Convert toDelete indices to mesh indices
	std::transform(toDelete.begin(), toDelete.end(), meshesToMerge.begin(), [&scene](uint32_t i) { return scene.meshes_.at(i); });

	// TODO: if merged mesh transforms are non-zero, then we should pre-transform individual mesh vertices in meshData using local transform

	// old-to-new mesh indices
	std::map<uint32_t, uint32_t> oldToNew;

	// now move all the meshesToMerge to the end of array
	mergeIndexArray(meshData, meshesToMerge, oldToNew);

	// cutoff all but one of the merged meshes (insert the last saved mesh from meshesToMerge - they are all the same)
	eraseSelected(meshData.meshes_, meshesToMerge);

	for (auto& n: scene.meshes_)
		n.second = oldToNew[n.second];

	// reattach the node with merged meshes [identity transforms are assumed]
	int newNode = addNode(scene, 0, 1);
	scene.meshes_[newNode] = meshData.meshes_.size() - 1;
	scene.materialForNode_[newNode] = (uint32_t)oldMaterial;

	deleteSceneNodes(scene, toDelete);
}

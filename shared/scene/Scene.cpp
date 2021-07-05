#include "shared/scene/Scene.h"
#include "shared/Utils.h"

#include <algorithm>
#include <numeric>

void saveStringList(FILE* f, const std::vector<std::string>& lines);
void loadStringList(FILE* f, std::vector<std::string>& lines);

int addNode(Scene& scene, int parent, int level)
{
	int node = (int)scene.hierarchy_.size();
	{
		// TODO: resize aux arrays (local/global etc.)
		scene.localTransform_.push_back(glm::mat4(1.0f));
		scene.globalTransform_.push_back(glm::mat4(1.0f));
	}
	scene.hierarchy_.push_back({ .parent_ = parent, .lastSibling_ = -1 });
	if (parent > -1)
	{
		// find first item (sibling)
		int s = scene.hierarchy_[parent].firstChild_;
		if (s == -1)
		{
			scene.hierarchy_[parent].firstChild_ = node;
			scene.hierarchy_[node].lastSibling_ = node;
		} else
		{
			int dest = scene.hierarchy_[s].lastSibling_;
			if (dest <= -1)
			{
				// no cached lastSibling, iterate nextSibling indices
				for (dest = s; scene.hierarchy_[dest].nextSibling_ != -1; dest = scene.hierarchy_[dest].nextSibling_);
			}
			scene.hierarchy_[dest].nextSibling_ = node;
			scene.hierarchy_[s].lastSibling_ = node;
		}
	}
	scene.hierarchy_[node].level_ = level;
	scene.hierarchy_[node].nextSibling_ = -1;
	scene.hierarchy_[node].firstChild_  = -1;
	return node;
}

void markAsChanged(Scene& scene, int node)
{
	int level = scene.hierarchy_[node].level_;
	scene.changedAtThisFrame_[level].push_back(node);

	// TODO: use non-recursive iteration with aux stack
	for (int s = scene.hierarchy_[node].firstChild_; s != - 1 ; s = scene.hierarchy_[s].nextSibling_)
		markAsChanged(scene, s);
}

int findNodeByName(const Scene& scene, const std::string& name)
{
	// Extremely simple linear search without any hierarchy reference
	// To support DFS/BFS searches separate traversal routines are needed

	for (size_t i = 0 ; i < scene.localTransform_.size() ; i++)
		if (scene.nameForNode_.contains(i))
		{
			int strID = scene.nameForNode_.at(i);
			if (strID > -1)
				if (scene.names_[strID] == name)
					return (int)i;
		}

	return -1;
}

int getNodeLevel(const Scene& scene, int n)
{
	int level = -1;
	for (int p = 0 ; p != -1 ; p = scene.hierarchy_[p].parent_, level++);
	return level;
}

bool mat4IsIdentity(const glm::mat4& m);
void fprintfMat4(FILE* f, const glm::mat4& m);

// CPU version of global transform update []
void recalculateGlobalTransforms(Scene& scene)
{
	if (!scene.changedAtThisFrame_[0].empty())
	{
		int c = scene.changedAtThisFrame_[0][0];
		scene.globalTransform_[c] = scene.localTransform_[c];
		scene.changedAtThisFrame_[0].clear();
	}

	for (int i = 1 ; i < MAX_NODE_LEVEL && (!scene.changedAtThisFrame_[i].empty()); i++ )
	{
		for (const int& c: scene.changedAtThisFrame_[i])
		{
			int p = scene.hierarchy_[c].parent_;
			scene.globalTransform_[c] = scene.globalTransform_[p] * scene.localTransform_[c];
		}
		scene.changedAtThisFrame_[i].clear();
	}
}

void loadMap(FILE* f, std::unordered_map<uint32_t, uint32_t>& map)
{
	std::vector<uint32_t> ms;

	uint32_t sz = 0;
	fread(&sz, 1, sizeof(sz), f);

	ms.resize(sz);
	fread(ms.data(), sizeof(int), sz, f);
	for (size_t i = 0; i < (sz / 2) ; i++)
		map[ms[i * 2 + 0]] = ms[i * 2 + 1];
}

void loadScene(const char* fileName, Scene& scene)
{
	FILE* f = fopen(fileName, "rb");

	if (!f)
	{
		printf("Cannot open scene file '%s'. Please run SceneConverter from Chapter7 and/or MergeMeshes from Chapter 9", fileName);
		return;
	}

	uint32_t sz = 0;
	fread(&sz, sizeof(sz), 1, f);

	scene.hierarchy_.resize(sz);
	scene.globalTransform_.resize(sz);
	scene.localTransform_.resize(sz);
	// TODO: check > -1
	// TODO: recalculate changedAtThisLevel() - find max depth of a node [or save scene.maxLevel]
	fread(scene.localTransform_.data(), sizeof(glm::mat4), sz, f);
	fread(scene.globalTransform_.data(), sizeof(glm::mat4), sz, f);
	fread(scene.hierarchy_.data(), sizeof(Hierarchy), sz, f);

	// Mesh for node [index to some list of buffers]
	loadMap(f, scene.materialForNode_);
	loadMap(f, scene.meshes_);

	if (!feof(f))
	{
		loadMap(f, scene.nameForNode_);
		loadStringList(f, scene.names_);

		loadStringList(f, scene.materialNames_);
	}

	fclose(f);
}

void saveMap(FILE* f, const std::unordered_map<uint32_t, uint32_t>& map)
{
	std::vector<uint32_t> ms;
	ms.reserve(map.size() * 2);
	for (const auto& m : map)
	{
		ms.push_back(m.first);
		ms.push_back(m.second);
	}
	const uint32_t sz = static_cast<uint32_t>(ms.size());
	fwrite(&sz, sizeof(sz), 1, f);
	fwrite(ms.data(), sizeof(int), ms.size(), f);
}

void saveScene(const char* fileName, const Scene& scene)
{
	FILE* f = fopen(fileName, "wb");

	const uint32_t sz = (uint32_t)scene.hierarchy_.size();
	fwrite(&sz, sizeof(sz), 1, f);

	fwrite(scene.localTransform_.data(), sizeof(glm::mat4), sz, f);
	fwrite(scene.globalTransform_.data(), sizeof(glm::mat4), sz, f);
	fwrite(scene.hierarchy_.data(), sizeof(Hierarchy), sz, f);

	// Mesh for node [index to some list of buffers]
	saveMap(f, scene.materialForNode_);
	saveMap(f, scene.meshes_);

	if (!scene.names_.empty() && !scene.nameForNode_.empty())
	{
		saveMap(f, scene.nameForNode_);
		saveStringList(f, scene.names_);

		saveStringList(f, scene.materialNames_);
	}
	fclose(f);
}

bool mat4IsIdentity(const glm::mat4& m)
{
	return (m[0][0] == 1 && m[0][1] == 0 && m[0][2] == 0 && m[0][3] == 0 &&
		m[1][0] == 0 && m[1][1] == 1 && m[1][2] == 0 && m[1][3] == 0 &&
		m[2][0] == 0 && m[2][1] == 0 && m[2][2] == 1 && m[2][3] == 0 &&
		m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1);
}

void fprintfMat4(FILE* f, const glm::mat4& m)
{
	if (mat4IsIdentity(m))
	{
		fprintf(f, "Identity\n");
	}
	else
	{
		fprintf(f, "\n");
		for (int i = 0 ; i < 4 ; i++) {
			for (int j = 0 ; j < 4 ; j++)
				fprintf(f, "%f ;", m[i][j]);
			fprintf(f, "\n");
		}
	}
}

void dumpTransforms(const char* fileName, const Scene& scene)
{
	FILE* f = fopen(fileName, "a+");
	for (size_t i = 0 ; i < scene.localTransform_.size() ; i++)
	{
		fprintf(f, "Node[%d].localTransform: ", (int)i);
		fprintfMat4(f, scene.localTransform_[i]);
		fprintf(f, "Node[%d].globalTransform: ", (int)i);
		fprintfMat4(f, scene.globalTransform_[i]);
		fprintf(f, "Node[%d].globalDet = %f; localDet = %f\n", (int)i, glm::determinant(scene.globalTransform_[i]), glm::determinant(scene.localTransform_[i]));
	}
	fclose(f);
}

void printChangedNodes(const Scene& scene)
{
	for (int i = 0 ; i < MAX_NODE_LEVEL && (!scene.changedAtThisFrame_[i].empty()); i++ )
	{
		printf("Changed at level(%d):\n", i);

		for (const int& c: scene.changedAtThisFrame_[i])
		{
			int p = scene.hierarchy_[c].parent_;
			//scene.globalTransform_[c] = scene.globalTransform_[p] * scene.localTransform_[c];
			printf(" Node %d. Parent = %d; LocalTransform: ", c, p);
			fprintfMat4(stdout, scene.localTransform_[i]);
			if (p > -1)
			{
				printf(" ParentGlobalTransform: ");
				fprintfMat4(stdout, scene.globalTransform_[p]);
			}
		}
	}
}

// Shift all hierarchy components in the nodes
void shiftNodes(Scene& scene, int startOffset, int nodeCount, int shiftAmount)
{
	auto shiftNode = [shiftAmount](Hierarchy& node)
	{
		if (node.parent_ > -1)
			node.parent_ += shiftAmount;
		if (node.firstChild_ > -1)
			node.firstChild_ += shiftAmount;
		if (node.nextSibling_ > -1)
			node.nextSibling_ += shiftAmount;
		if (node.lastSibling_ > -1)
			node.lastSibling_ += shiftAmount;
		// node->level_ does not have to be shifted
	};

	// If there are too many nodes, we can use std::execution::par with std::transform
//	std::transform(scene.hierarchy_.begin() + startOffset, scene.hierarchy_.begin() + nodeCount, scene.hierarchy_.begin() + startOffset, shiftNode);

//	for (auto i = scene.hierarchy_.begin() + startOffset ; i != scene.hierarchy_.begin() + nodeCount ; i++)
//		shiftNode(*i);

	for (int i = 0 ; i < nodeCount ; i++)
		shiftNode(scene.hierarchy_[i + startOffset]);
}

using ItemMap = std::unordered_map<uint32_t, uint32_t>;

// Add the items from otherMap shifting indices and values along the way
void mergeMaps(ItemMap& m, const ItemMap& otherMap, int indexOffset, int itemOffset)
{
	for (const auto& i: otherMap)
		m[i.first + indexOffset] = i.second + itemOffset;
}

/**
	There are different use cases for scene merging.
	The simplest one is the direct "gluing" of multiple scenes into one [all the material lists and mesh lists are merged and indices in all scene nodes are shifted appropriately]
	The second one is creating a "grid" of objects (or scenes) with the same material and mesh sets.
	For the second use case we need two flags: 'mergeMeshes' and 'mergeMaterials' to avoid shifting mesh indices
*/
void mergeScenes(Scene& scene, const std::vector<Scene*>& scenes, const std::vector<glm::mat4>& rootTransforms, const std::vector<uint32_t>& meshCounts,
	bool mergeMeshes, bool mergeMaterials)
{
	// Create new root node
	scene.hierarchy_ = {
		{
			.parent_ = -1,
			.firstChild_ = 1,
			.nextSibling_ = -1,
			.lastSibling_ = -1,
			.level_ = 0
		}
	};

	scene.nameForNode_[0] = 0;
	scene.names_ = { "NewRoot" };

	scene.localTransform_.push_back(glm::mat4(1.f));
	scene.globalTransform_.push_back(glm::mat4(1.f));

	if (scenes.empty())
		return;

	int offs = 1;
	int meshOffs = 0;
	int nameOffs = (int)scene.names_.size();
	int materialOfs = 0;
	auto meshCount = meshCounts.begin();

	if (!mergeMaterials)
		scene.materialNames_ = scenes[0]->materialNames_;

	// FIXME: too much logic (for all the components in a scene, though mesh data and materials go separately - there are dedicated data lists)
	for (const Scene* s: scenes)
	{
		mergeVectors(scene.localTransform_, s->localTransform_);
		mergeVectors(scene.globalTransform_, s->globalTransform_);

		mergeVectors(scene.hierarchy_, s->hierarchy_);

		mergeVectors(scene.names_, s->names_);
		if (mergeMaterials)
			mergeVectors(scene.materialNames_, s->materialNames_);

		int nodeCount = (int)s->hierarchy_.size();

		shiftNodes(scene, offs, nodeCount, offs);

		mergeMaps(scene.meshes_,          s->meshes_,          offs, mergeMeshes ? meshOffs : 0);
		mergeMaps(scene.materialForNode_, s->materialForNode_, offs, mergeMaterials ? materialOfs : 0);
		mergeMaps(scene.nameForNode_,     s->nameForNode_,     offs, nameOffs);

		offs += nodeCount;

		materialOfs += (int)s->materialNames_.size();
		nameOffs += (int)s->names_.size();

		if (mergeMeshes)
		{
			meshOffs += *meshCount;
			meshCount++;
		}
	}

	// fixing 'nextSibling' fields in the old roots (zero-index in all the scenes)
	offs = 1;
	int idx = 0;
	for (const Scene* s: scenes)
	{
		int nodeCount = (int)s->hierarchy_.size();
		bool isLast = (idx == scenes.size() - 1);
		// calculate new next sibling for the old scene roots
		int next = isLast ? -1 : offs + nodeCount;
		scene.hierarchy_[offs].nextSibling_ = next;
		// attach to new root
		scene.hierarchy_[offs].parent_ = 0;

		// transform old root nodes, if the transforms are given
		if (!rootTransforms.empty())
			scene.localTransform_[offs] = rootTransforms[idx] * scene.localTransform_[offs];

		offs += nodeCount;
		idx++;
	}

	// now shift levels of all nodes below the root
	for (auto i = scene.hierarchy_.begin() + 1 ; i != scene.hierarchy_.end() ; i++)
		i->level_++;
}

void dumpSceneToDot(const char* fileName, const Scene& scene, int* visited)
{
	FILE* f = fopen(fileName, "w");
	fprintf(f, "digraph G\n{\n");
	for (size_t i = 0; i < scene.globalTransform_.size(); i++)
	{
		std::string name = "";
		std::string extra = "";
		if (scene.nameForNode_.contains(i))
		{
			int strID = scene.nameForNode_.at(i);
			name = scene.names_[strID];
		}
		if (visited)
		{
			if (visited[i])
				extra = ", color = red";
		}
		fprintf(f, "n%d [label=\"%s\" %s]\n", (int)i, name.c_str(), extra.c_str());
	}
	for (size_t i = 0; i < scene.hierarchy_.size(); i++)
	{
		int p = scene.hierarchy_[i].parent_;
		if (p > -1)
			fprintf(f, "\t n%d -> n%d\n", p, (int)i);
	}
	fprintf(f, "}\n");
	fclose(f);
}

/** A rather long algorithm (and the auxiliary routines) to delete a number of scene nodes from the hierarchy */
/* */

// Add an index to a sorted index array
static void addUniqueIdx(std::vector<uint32_t>& v, uint32_t index)
{
	if (!std::binary_search(v.begin(), v.end(), index))
		v.push_back(index);
}

// Recurse down from a node and collect all nodes which are already marked for deletion
static void collectNodesToDelete(const Scene& scene, int node, std::vector<uint32_t>& nodes)
{
	for (int n = scene.hierarchy_[node].firstChild_; n != - 1 ; n = scene.hierarchy_[n].nextSibling_) {
		addUniqueIdx(nodes, n);
		collectNodesToDelete(scene, n, nodes);
	}
}

int findLastNonDeletedItem(const Scene& scene, const std::vector<int>& newIndices, int node)
{
	// we have to be more subtle:
	//   if the (newIndices[firstChild_] == -1), we should follow the link and extract the last non-removed item
	//   ..
	if (node == -1)
		return -1;

	return (newIndices[node] == -1) ?
		findLastNonDeletedItem(scene, newIndices, scene.hierarchy_[node].nextSibling_) :
		newIndices[node];
}

void shiftMapIndices(std::unordered_map<uint32_t, uint32_t>& items, const std::vector<int>& newIndices)
{
	std::unordered_map<uint32_t, uint32_t> newItems;
	for (const auto& m: items) {
		int newIndex = newIndices[m.first];
		if (newIndex != -1)
			newItems[newIndex] = m.second;
	}
	items = newItems;
}

// Approximately an O ( N * Log(N) * Log(M)) algorithm (N = scene.size, M = nodesToDelete.size) to delete a collection of nodes from scene graph
void deleteSceneNodes(Scene& scene, const std::vector<uint32_t>& nodesToDelete)
{
	// 0) Add all the nodes down below in the hierarchy
	auto indicesToDelete = nodesToDelete;
	for (auto i: indicesToDelete)
		collectNodesToDelete(scene, i, indicesToDelete);

	// aux array with node indices to keep track of the moved ones [moved = [](node) { return (node != nodes[node]); ]
	std::vector<int> nodes(scene.hierarchy_.size());
	std::iota(nodes.begin(), nodes.end(), 0);

	// 1.a) Move all the indicesToDelete to the end of 'nodes' array (and cut them off, a variation of swap'n'pop for multiple elements)
	auto oldSize = nodes.size();
	eraseSelected(nodes, indicesToDelete);

	// 1.b) Make a newIndices[oldIndex] mapping table
	std::vector<int> newIndices(oldSize, -1);
	for(int i = 0 ; i < nodes.size() ; i++)
		newIndices[nodes[i]] = i;

	// 2) Replace all non-null parent/firstChild/nextSibling pointers in all the nodes by new positions
	auto nodeMover = [&scene, &newIndices](Hierarchy& h) {
		return Hierarchy {
			.parent_ = (h.parent_ != -1) ? newIndices[h.parent_] : -1,
			.firstChild_ = findLastNonDeletedItem(scene, newIndices, h.firstChild_),
			.nextSibling_ = findLastNonDeletedItem(scene, newIndices, h.nextSibling_),
			.lastSibling_ = findLastNonDeletedItem(scene, newIndices, h.lastSibling_)
		};
	};
	std::transform(scene.hierarchy_.begin(), scene.hierarchy_.end(), scene.hierarchy_.begin(), nodeMover);

	// 3) Finally throw away the hierarchy items
	eraseSelected(scene.hierarchy_, indicesToDelete);

	// 4) As in mergeScenes() routine we also have to adjust all the "components" (i.e., meshes, materials, names and transformations)

	// 4a) Transformations are stored in arrays, so we just erase the items as we did with the scene.hierarchy_
	eraseSelected(scene.localTransform_, indicesToDelete);
	eraseSelected(scene.globalTransform_, indicesToDelete);

	// 4b) All the maps should change the key values with the newIndices[] array
	shiftMapIndices(scene.meshes_, newIndices);
	shiftMapIndices(scene.materialForNode_, newIndices);
	shiftMapIndices(scene.nameForNode_, newIndices);

	// 5) scene node names list is not modified, but in principle it can be (remove all non-used items and adjust the nameForNode_ map)
	// 6) Material names list is not modified also, but if some materials fell out of use
}

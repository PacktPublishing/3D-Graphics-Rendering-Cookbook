#include "shared/scene/Scene.h"

#include <string>

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
			scene.hierarchy_[s].lastSibling_ = node;
			scene.hierarchy_[dest].nextSibling_ = node;
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
		printf("Cannot open file '%s'.", fileName);
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

#include "Scene.h"


Scene::Scene()
{
	PAF_FILE_LINE;
	m_rootNode = Node::New();
	m_rootNode->m_scene = this;
}

pafcore::UniquePtr<Scene> Scene::New()
{
	auto scene = pafcore::UniquePtr<Scene>::Make();
	return scene;
}

void Scene::check__()
{
	m_rootNode->check__(this, nullptr, 0);
}


#include "Scene.h"
#include "Scene.mh"
#include "Scene.ic"
#include "Scene.mc"


Scene::Scene()
{
	m_rootNode = Node::New();
	m_rootNode->m_scene = this;
}

void Scene::check__()
{
	m_rootNode->check__(this, nullptr, 0);
}


#pragma once

#include "Node.h"
#include "../pafcore/SmartPtr.h"

class Scene
{
	friend class Node;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
public:
	Scene();
public:
	pafcore::BorrowedPtr<Node> Scene::getRootNode() const
	{
		return pafcore::BorrowedPtr<Node>(m_rootNode);
	}
public:
	void check__();
protected:
	pafcore::UniquePtr<Node> m_rootNode;
public:
	static pafcore::UniquePtr<Scene> New();
};

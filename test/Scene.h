//DO NOT EDIT THIS FILE, it is generated by idlcpp
//http://www.idlcpp.org

#pragma once

#include "../pafcore/Utility.h"
#include "../pafcore/SmartPtr.h"
class Node;
namespace paf{ class ClassType; }


class Scene : public paf::Introspectable
{
public:
	static ::paf::ClassType* GetType();
	::paf::ClassType* getType()
	{
		return GetType();
	}
	void* getAddress()
	{
		return this;
	}

	Scene();

	static ::paf::UniquePtr<Scene> New();


	friend class Node;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
public:
	paf::BorrowedPtr<Node> Scene::getRootNode() const
	{
		return paf::BorrowedPtr<Node>(m_rootNode);
	}
public:
	void check__();
protected:
	paf::UniquePtr<Node> m_rootNode;

};

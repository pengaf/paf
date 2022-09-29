#pragma once

#include "../pafcore/SmartPtr.h"
#include "../pafcore/Object.h"
#include <string>
//template<typename T>
//class Iterator
//{
//public:
//	bool valid();
//	T get();
//	void advance();
//};

class Scene;
class Node
{
public:
	pafcore::BorrowedPtr<Node> addChild(pafcore::UniquePtr<Node>& node)
	{
		return PushBack(std::move(node), pafcore::BorrowedPtr<Node>(this));
	}

	pafcore::BorrowedPtr<Node> addSibling(pafcore::UniquePtr<Node>&& node)
	{
		return InsertBefore(std::move(node), pafcore::BorrowedPtr<Node>(this));
	}
	bool isFreeNode() const
	{
		return !bool(m_parent);
	}
	Node()
	{
		m_name = "AAAA";
	}
	virtual ~Node()
	{
	}
public:
	void check__(Scene* scene, Node* parent, int depth);
public:
	pafcore::UniquePtr<Node> m_nextSibling;
	pafcore::BorrowedPtr<Node> m_prevSibling;
	pafcore::UniquePtr<Node> m_childHead;
	pafcore::BorrowedPtr<Node> m_childTail;
	pafcore::BorrowedPtr<Node> m_parent;
	pafcore::BorrowedPtr<Scene> m_scene;
	std::string m_name;
public:
	static pafcore::UniquePtr<Node> New()
	{
		return pafcore::UniquePtr<Node>::Make();
	}
	static pafcore::BorrowedPtr<Node> PushBack(pafcore::UniquePtr<Node>&& node, pafcore::BorrowedPtr<Node> parentNode);
	static pafcore::BorrowedPtr<Node> InsertBefore(pafcore::UniquePtr<Node>&& node, pafcore::BorrowedPtr<Node> siblingNode);
	static pafcore::UniquePtr<Node> RemoveFromParent(pafcore::BorrowedPtr<Node> node);
};

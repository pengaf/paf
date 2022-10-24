#import "../pafcore/Utility.i"

#{
#include <string>
class Scene;
#}

class Node : Introspectable
{
	Node();
	Node+ addChild(Node! && node);
	Node+ addSibling(Node! && node);
	bool isFreeNode() const;
#{
public:
	void check__(Scene* scene, Node* parent, int depth);
public:
	paf::UniquePtr<Node> m_nextSibling;
	paf::BorrowedPtr<Node> m_prevSibling;
	paf::UniquePtr<Node> m_childHead;
	paf::BorrowedPtr<Node> m_childTail;
	paf::BorrowedPtr<Node> m_parent;
	paf::BorrowedPtr<Scene> m_scene;
	std::string m_name;
public:
	static paf::BorrowedPtr<Node> PushBack(paf::UniquePtr<Node>&& node, paf::BorrowedPtr<Node> parentNode);
	static paf::BorrowedPtr<Node> InsertBefore(paf::UniquePtr<Node>&& node, paf::BorrowedPtr<Node> siblingNode);
	static paf::UniquePtr<Node> RemoveFromParent(paf::BorrowedPtr<Node> node);
#}
};

#{
inline paf::BorrowedPtr<Node> Node::addChild(paf::UniquePtr<Node>&& node)
{
	return PushBack(std::move(node), paf::BorrowedPtr<Node>(this));
}

inline paf::BorrowedPtr<Node> Node::addSibling(paf::UniquePtr<Node>&& node)
{
	return InsertBefore(std::move(node), paf::BorrowedPtr<Node>(this));
}

inline bool Node::isFreeNode() const
{
	return !bool(m_parent);
}

#}
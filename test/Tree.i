#import "../pafcore/Utility.i"
#{
#include "../pafcore/SmartPtr.h"
#}

class Tree : paf::Introspectable
{
	class Node : paf::Introspectable
	{
		bool addChild(Node* node, Node* nextSibling#{ = nullptr#});
#{
	protected:
		paf::SharedPtr<Node> m_nextSibling;
		paf::WeakPtr<Node> m_prevSibling;
		paf::SharedPtr<Node> m_childHead;
		paf::WeakPtr<Node> m_childTail;
		paf::WeakPtr<Node> m_parent;
		paf::WeakPtr<Tree> m_tree;
#}
	};

	Node^ getRoot() const;
	void setRoot(Node* node);
#{
public:
	Node* getRoot_() const;
protected:
	paf::SharedPtr<Node> m_root;
#}
};


#{

inline paf::SharedPtr<Tree::Node> Tree::getRoot() const
{
	return m_root;
}

inline Tree::Node* Tree::getRoot_() const
{
	return m_root;
}

#}

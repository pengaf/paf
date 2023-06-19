#import "../pafcore/Utility.i"
#{
#include "../pafcore/SmartPtr.h"
#}

#{
class Tree;
#}

class TreeNode : paf::Introspectable
{
	bool isFree() const;
	TreeNode^ getFirstChild() const;
	TreeNode^ getNextSibling() const;
	bool addChild(TreeNode* node, TreeNode* nextSibling#{ = nullptr#});
#{
	friend class Tree;
public:
	//Tree* getTree_() const;
	TreeNode* getFirstChild_() const;
	TreeNode* getNextSibling_() const;
protected:
	void addFreeChild(TreeNode* child, TreeNode* nextSibling);
protected:
	paf::SharedPtr<TreeNode> m_nextSibling;
	paf::WeakPtr<TreeNode> m_prevSibling;
	paf::SharedPtr<TreeNode> m_childHead;
	paf::WeakPtr<TreeNode> m_childTail;
	paf::WeakPtr<TreeNode> m_parent;
	//paf::WeakPtr<Tree> m_tree;
#}
};


class Tree : paf::Introspectable
{
	TreeNode^ getRoot() const;
	void setRoot(TreeNode* node);
	bool attachNode(TreeNode* node, TreeNode* parent#{ = nullptr#}, TreeNode* nextSibling#{ = nullptr#});
	bool detachNode(TreeNode* node);
#{
	friend class TreeNode;
public:
	TreeNode* getRoot_() const;
protected:
	paf::SharedPtr<TreeNode> m_root;
#}
};


#{

inline paf::SharedPtr<TreeNode> TreeNode::getFirstChild() const
{
	return m_childHead;
}

inline paf::SharedPtr<TreeNode> TreeNode::getNextSibling() const
{
	return m_nextSibling;
}

inline TreeNode* TreeNode::getFirstChild_() const
{
	return m_childHead.get();
}

inline TreeNode* TreeNode::getNextSibling_() const
{
	return m_nextSibling.get();
}


inline bool TreeNode::isFree() const
{
	return !m_parent;
}

inline Tree* TreeNode::getTree_() const
{
	return m_tree.get();
}

inline paf::SharedPtr<TreeNode> Tree::getRoot() const
{
	return m_root;
}

inline TreeNode* Tree::getRoot_() const
{
	return m_root.get();
}

#}

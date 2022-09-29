#include <iostream>
#include <memory>
#include <assert.h>
#include <vector>
#include <string>
#include <tuple>
#include "Scene.h"
#include "Node.h"
#include "../pafcore/SmartPtr.h"
#include "../pafcore/System.h"

//
//#ifdef _DEBUG
//	#define PAF_FILE_LINE(...) PAF_WIDE(__FILE__), __LINE__, __VA_ARGS__
//#else
//	#define PAF_FILE_LINE(...) __VA_ARGS__
//#endif // _DEBUG


//pafcore::UniquePtr<Node> m_nextSibling;
//pafcore::BorrowedPtr<Node> m_prevSibling;
//pafcore::UniquePtr<Node> m_childHead;
//pafcore::BorrowedPtr<Node> m_childTail;
//pafcore::BorrowedPtr<Node> m_parent;
//pafcore::BorrowedPtr<Scene> m_scene;


void test1()
{
	PAF_FILE_LINE;
	auto scene = Scene::New();
	for (int i = 0; i < 5; ++i)
	{
		PAF_FILE_LINE; 
		auto node = Node::New();
		char name[222];
		sprintf(name, "%d", i);
		node->m_name = name;
		auto bn = scene->getRootNode()->addChild(node);
		for (int j = 0; j < 5; ++j)
		{
			PAF_FILE_LINE; auto node = Node::New();
			sprintf(name, "%d-%d", i,j);
			node->m_name = name;
			if (bn->m_childHead && j > 2)
			{
				bn->m_childHead->m_nextSibling->m_nextSibling->addSibling(std::move(node));
			}
			else
			{
				bn->addChild(std::move(node));
			}
		}

	}
	scene->check__();


	std::cout << sizeof(pafcore::UniquePtr<Node>) << "  " << sizeof(pafcore::SharedPtr<Node>) << "  " << sizeof(pafcore::BorrowedPtr<Node>);
}

class AA
{
public:
	int m_int{ 33 };
};

//void test2(int&& a)
//{
//	//a = 3;
//}
//void test2(const int& a)
//{
//	//a = 3;
//}
//
//void test2(int a)
//{
//	a = 3;
//}

class Node2 : public Node
{
public:
	~Node2()
	{
		printf("~Node2()\n");
	}
	void* aaa;
};

//int@ NewUnique();
//int^ NewShared();
//int*- Get();
//void ReleaseOwnership()
//template <>
//class max_size<>
//{
//	static constexpr size_t value = 0;
//};


int main()
{
	{
		std::vector<Node2> vv;
		vv.push_back(Node2());
		vv.push_back(Node2());
	}
	new int[3];
	//const char[] aa = "aaaa";
	std::tuple<int, char> tt;
	{
		//pafcore::UniqueArray<int> aa;
		{
			auto sn = pafcore::SharedArray<int>::Make(3);
			//pafcore::BorrowedArray<int> xx(sn);
			//aa = std::move(sn);
		}
	}
	{
		///auto aa = std::make_shared<int[]>(3,3);

		std::shared_ptr<int[]> p3(new int[3]);
	}
	{
		//Node2* aa;
		//{
		//	auto* sn = new Node;
		//	aa = (Node2*)sn;
		//}
		//delete aa;
	}

	//AA cc(aa);

	//scene.addHead(node);

	//std::string str("aaaaaa");
	//std::vector<std::string> strs;
	//strs.push_back(std::move(str));
	//{
	//	PAF_FILE_LINE(auto p1 = pafcore::UniquePtr<int>::Make(3));
	//	PAF_FILE_LINE(auto p2 = pafcore::UniquePtr<int>::Make(3));
	//	p1 = std::move(p2);
	//}
	//{
	//	auto p1 = pafcore::UniquePtr<int>::Make(3);
	//	auto p2 = pafcore::BorrowedPtr<int>(p1);
	//}
	//{
	//	std::unique_ptr<int[]> p1(new int[100000]);
		//std::unique_ptr<int> p2(new int(4));
		//std::weak_ptr<int> p4(p3);
		//auto aa = std::make_shared<AA>();
		//auto aa2 = aa->shared_from_this();
		{
			//auto bb = std::shared_ptr<int[]>(new int[3]);
			//auto cc = std::make_shared<int[]>();
		}
		//	std::weak_ptr<int> p4(p3);
	//	std::cout << sizeof(p1) << "  " << sizeof(p2) << "  " << sizeof(p3) << "\n\n";
	//}
	////pafcore::System::DebugBreak();
	//{
	//	pafcore::UniquePtr<int> ptr(new int);
	//	std::cout << sizeof(ptr);
	//}
	test1();
	return 0;
}


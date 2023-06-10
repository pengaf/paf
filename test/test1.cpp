#include <iostream>
#include <memory>
#include <assert.h>
#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include "Scene.h"
#include "Node.h"
#include "../pafcore/SmartPtr.h"
#include "../pafcore/System.h"
#include "../pafcore/String.h"
#pragma comment(lib, "pafcore_d.lib")
//
//#ifdef _DEBUG
//	#define PAF_FILE_LINE(...) PAF_WIDE(__FILE__), __LINE__, __VA_ARGS__
//#else
//	#define PAF_FILE_LINE(...) __VA_ARGS__
//#endif // _DEBUG


//paf::UniquePtr<Node> m_nextSibling;
//paf::BorrowedPtr<Node> m_prevSibling;
//paf::UniquePtr<Node> m_childHead;
//paf::BorrowedPtr<Node> m_childTail;
//paf::BorrowedPtr<Node> m_parent;
//paf::BorrowedPtr<Scene> m_scene;


void test1()
{
	auto scene = Scene::New();
	for (int i = 0; i < 5; ++i)
	{
		auto node = Node::New();
		char name[222];
		sprintf(name, "%d", i);
		node->m_name = name;
		auto bn = scene->getRootNode()->addChild(std::move(node));
		for (int j = 0; j < 5; ++j)
		{
			auto node = Node::New();
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


	std::cout << sizeof(paf::UniquePtr<Node>) << "  " << sizeof(paf::SharedPtr<Node>) << "  " << sizeof(paf::BorrowedPtr<Node>);
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

//class Node2 : public Node
//{
//public:
//	~Node2()
//	{
//		printf("~Node2()\n");
//	}
//	void* aaa;
//};
//
//int@ NewUnique();
//int^ NewShared();
//int*- Get();
//void ReleaseOwnership()
//template <>
//class max_size<>
//{
//	static constexpr size_t value = 0;
//};

class Test
{
public:
	Test(int a=1, int b=1)
	{}
public:
	void test1(int& a)
	{
		std::cout << "test1(int& a)\n";
	}

	void test2(int && a) 
	{
		//a = 3;// std::make_unique<int>(4);
		std::cout << "test1(int&& a)\n";
	}

	void test1(const int& a) 
	{
		std::cout << "test1(const int& a)\n";
	}
};

const int* testParam(int & a, const int& b, int const && c)
{
	return &c;
}

int main()
{
	auto fn = std::mem_fn(&std::string::length);
	std::function<void()> fn2;
	int a = sizeof(std::string);
	Test test;
	std::unique_ptr<int> p;// = std::make_unique<int>(3);
	float nn{};
	//test.test2(nn);
	test.test2(std::move(nn));

	std::vector<Test> vv(3);
	//{
	//	std::vector<Node2> vv;
	//	vv.push_back(Node2());
	//	vv.push_back(Node2());
	//}
	new int[3];
	//const char[] aa = "aaaa";
	std::tuple<int, char> tt;
	{
		//paf::UniqueArray<int> aa;
		{
			auto sn = paf::SharedArray<int>::Make(3);
			//paf::BorrowedArray<int> xx(sn);
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
	//	PAF_FILE_LINE(auto p1 = paf::UniquePtr<int>::Make(3));
	//	PAF_FILE_LINE(auto p2 = paf::UniquePtr<int>::Make(3));
	//	p1 = std::move(p2);
	//}
	//{
	//	auto p1 = paf::UniquePtr<int>::Make(3);
	//	auto p2 = paf::BorrowedPtr<int>(p1);
	//}
	//{
	//	std::unique_ptr<int[]> p1(new int[100000]);
		std::unique_ptr<int> p2(new int(4));
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
	////paf::System::DebugBreak();
	//{
	//	paf::UniquePtr<int> ptr(new int);
	//	std::cout << sizeof(ptr);
	//}
	test1();
	return 0;
}


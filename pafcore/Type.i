#import "Metadata.i"

namespace pafcore
{
	abstract class #PAFCORE_EXPORT Type : Metadata
	{
		size_t _size_ { get };
#{
	public:
		Type(const char* name, MetaCategory category, const char* declarationFile);
		~Type();
	public:
		virtual void destroyInstance(void* address);
		virtual void destroyArray(void* address);
		virtual bool assign(void* dst, const void* src);
		virtual Metadata* findMember(const char* name) = 0;
	public:
		bool isPrimitive() const;
		bool isEnum() const;
		bool isObject() const;
		bool isIntrospectionObject() const;
		bool isClass() const;
		const char* getDeclarationFile() const;
	public:
		MetaCategory m_category;
		size_t m_size;
		Metadata* m_enclosing;
		const char* m_declarationFile;//类型声明所在文件路径
#}
	};

#{

	inline size_t Type::get__size_() const
	{
		return m_size;
	}

	inline bool Type::isPrimitive() const
	{
		return primitive == m_category;
	}
	
	inline bool Type::isEnum() const
	{
		return enumeration == m_category;
	}
	
	inline bool Type::isObject() const
	{
		return class_object == m_category;
	}
	
	inline bool Type::isIntrospectionObject() const
	{
		return introspection_class_object <= m_category;
	}

	inline bool Type::isClass() const
	{
		return class_object <= m_category;
	}
	
	inline const char* Type::getDeclarationFile() const
	{
		return m_declarationFile;
	}

#}

}

function Indentation(count)
	res = "";
	for i = 1, count do
		res = res .. "\t";
	end
	return res;
end

function ListPrimitiveMember(var, indent, recursive)
	count = var:_getMemberCount_();
	for i = 1, count do
		member = var:_getMember_(i - 1);
		print(Indentation(indent) .. member._name_ .. " : " .. idlcpp.paf.MetaCategory:_getEnumeratorByValue_(member._metaCategory_)._name_);
	end
end

function ListEnumMember(var, indent, recursive)
	count = var:_getEnumeratorCount_();
	for i = 1, count do
		member = var:_getEnumerator_(i - 1);
		print(Indentation(indent) .. member._name_ .. " : " .. member._value_);
	end
end

function ListClassMember(var, indent, recursive)
	count = var:_getBaseClassCount_();
	for i = 1, count do
		baseClass = var:_getBaseClass_(i - 1);
		print(Indentation(indent) .. baseClass._name_);
	end
	count = var:_getMemberCount_(false);
	for i = 1, count do
		member = var:_getMember_(i - 1, false);
		print(Indentation(indent) .. member._name_ .. " : " .. idlcpp.paf.MetaCategory:_getEnumeratorByValue_(member._metaCategory_)._name_);
		if recursive then
			ListMember(member, indent + 1, recursive);
		end
	end
end

function ListNamespaceMember(var, indent, recursive)
	count = var:_getMemberCount_();
	for i = 1, count do
		member = var:_getMember_(i - 1);
		print(Indentation(indent) .. member._name_ .. " : " .. idlcpp.paf.MetaCategory:_getEnumeratorByValue_(member._metaCategory_)._name_);
		if recursive then
			ListMember(member, indent + 1, recursive);
		end
	end
end;

tab = {}
tab[idlcpp.paf.MetaCategory.name_space] = ListNamespaceMember;
tab[idlcpp.paf.MetaCategory.object_type] = ListClassMember;
tab[idlcpp.paf.MetaCategory.primitive_type] = ListPrimitiveMember;
tab[idlcpp.paf.MetaCategory.enumeration_type] = ListEnumMember;

function ListMember(var, indent, recursive)
	func = tab[var._metaCategory_]
	if func then
		func(var, indent, recursive);
	end
end

function ListDerivedClass(var)
	it = var:_getFirstDerivedClass_();
	while (it)
	do
		cls = it:deref();
		print(cls._name_);
		it = it:next();
	end
end

--print(paf.pafcore.Category.name_space._name_);
ListMember(idlcpp,0,0);
--ListDerivedClass(paf.pafcore.Metadata);
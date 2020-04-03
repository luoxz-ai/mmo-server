#ifndef XML2STRUCT_H
#define XML2STRUCT_H

#include <functional>
#include "tinyxml2/tinyxml2.h"

#include "StaticReflection.h"

namespace xml2struct
{

//forward decal
template<class T>
inline void xmlElement_to_struct(tinyxml2::XMLElement* pE, T& refStruct);

template<class FieldType>
inline void xml_value_to_field(tinyxml2::XMLElement* pVarE, FieldType* field)
{
    if constexpr (std::is_integral<FieldType>::value || std::is_floating_point<FieldType>::value || std::is_same<bool, FieldType>::value)
    {
        pVarE->QueryAttribute("val", field);
    }
    else if constexpr(std::is_same<std::string, FieldType>::value)
    {
        const char* pVal = pVarE->Attribute("val");
        if(pVal)
        {
            *field = pVal;
        }
    }
    else
    {
        xmlElement_to_struct(pVarE, *field);
    }
}

template<class FieldType>
inline void xml_value_to_field(tinyxml2::XMLElement* pVarE, FieldType* field,
                        std::function<void(tinyxml2::XMLElement*, FieldType*)>& after_func)
{
    xml_value_to_field(pVarE, field);
    after_func(pVarE, field);
}

struct ForEachXMLLambda
{
    tinyxml2::XMLElement* pVarE;
    const char*           field_name;
    std::size_t           field_name_hash;
    template<typename FieldInfo, typename Field>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field) const
    {
        // printf("test:%s\n", std::get<0>(this_field_info) );
        if(field_name_hash != std::get<1>(this_field_info))
            return false;
        // printf("vist:%s\n", std::get<0>(this_field_info));
        xml_value_to_field(pVarE, &this_field);
        return true;
    }

    template<typename FieldInfo, typename Field, typename Tag>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field, Tag&& tag) const
    {
        // printf("test:%s\n", std::get<0>(this_field_info));
        if(field_name_hash != std::get<1>(this_field_info))
            return false;
        // printf("vist:%s\n", std::get<0>(this_field_info));
        xml_value_to_field(pVarE, &this_field, std::forward<Tag>(tag));
        return true;
    }
};

template<class T>
inline void xmlElement_to_struct(tinyxml2::XMLElement* pE, T& refStruct)
{
    tinyxml2::XMLElement* pVarE = pE->FirstChildElement();
    while(pVarE != NULL)
    {
        const char* pStrName = pVarE->Attribute("name");
        if(pStrName != NULL)
        {
            std::string field_name      = pStrName;
            std::size_t field_name_hash = hash::MurmurHash3::shash(field_name.c_str(), field_name.size(), 0);
            static_reflection::FindInField(refStruct, ForEachXMLLambda{pVarE, field_name.c_str(), field_name_hash});
        }

        pVarE = pVarE->NextSiblingElement();
    }
}

template<class StructT>
inline bool xmlfile2struct(const char* pstrFileName, std::vector<StructT>& testNodeList)
{
    tinyxml2::XMLDocument doc;
	if(doc.LoadFile(pstrFileName) != tinyxml2::XMLError::XML_SUCCESS)
	{
		return false;
	}

	auto pRootE = doc.FirstChildElement("Root");
	if(pRootE == NULL)
	{
		return false;
	}

	auto pNodesE = pRootE->FirstChildElement("Nodes");
	if(pNodesE == NULL)
	{
		return false;
	}

	auto pNodeE = pNodesE->FirstChildElement();
	while(pNodeE)
	{

		std::string struct_name = pNodeE->Name();
		StructT testNode;
		xmlElement_to_struct(pNodeE, testNode);
        testNodeList.emplace(std::move(testNode));
		pNodeE = pNodeE->NextSiblingElement();
	}
}


} // namespace xml2struct






#endif /* XML2STRUCT_H */

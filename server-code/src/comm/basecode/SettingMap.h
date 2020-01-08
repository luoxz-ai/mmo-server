#ifndef SettingMap_h__
#define SettingMap_h__

#include <map>
#include <string>
#include <vector>
#include "tinyxml2/tinyxml2.h"

class CSettingNode
{
public:
	CSettingNode() = default;
	

	std::vector<CSettingNode>& operator[](const std::string& name)
	{
		return m_setChild[name];
	}

	const std::vector<CSettingNode>& operator[](const std::string& name) const
	{
		auto itFind = m_setChild.find(name);
		if(itFind != m_setChild.end())
		{
			return itFind->second;
		}

		static std::vector<CSettingNode> s_Empty;
		return s_Empty;
	}

	std::string Query(const std::string& AttName) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
			return it->second;
		else
			return std::string();
	}

	bool Query(const std::string& AttName, std::string& val) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = it->second;
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, short& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = (short)std::stoi(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, unsigned short& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = (unsigned short)std::stoul(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, int& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stoi(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, unsigned int& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stoul(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, long& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stol(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, unsigned long& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stoul(it->second,0,_base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, long long& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stoll(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, unsigned long long& val, int _base = 10) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stoull(it->second, 0, _base);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, float& val) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stof(it->second, 0);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, double& val) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			val = std::stod(it->second, 0);
			return true;
		}
		return false;
	}

	bool Query(const std::string& AttName, bool& val) const
	{
		auto it = m_setAttrib.find(AttName);
		if (it != m_setAttrib.end())
		{
			const std::string& result = it->second;
			val = (result == std::string("TRUE") || result == std::string("true") || result == std::string("1"));
			return true;
		}
		return false;
	}

	int QueryInt(const std::string& AttName, int _base = 10) const
	{
		int val = 0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stoi(it->second, 0, _base);
			return val;
		}
		return val;
	}
	
	float QueryFloat(const std::string& AttName) const
	{
		float val = 0.0f;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stof(it->second, 0);
			return val;
		}
		return val;
	}

	double QueryDouble(const std::string& AttName) const
	{
		double val = 0.0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stod(it->second, 0);
			return val;
		}
		return val;
	}

	long QueryLong(const std::string& AttName, int _base = 10) const
	{
		long val = 0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stol(it->second, 0, _base);
			return val;
		}
		return val;
	}

	unsigned long QueryULong(const std::string& AttName, int _base = 10) const
	{
		unsigned long val = 0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stoul(it->second, 0, _base);
			return val;
		}
		return val;
	}

	long long QueryLongLong(const std::string& AttName, int _base = 10) const
	{
		long long val = 0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stoll(it->second, 0, _base);
			return val;
		}
		return val;
	}

	unsigned long long QueryULongLong(const std::string& AttName, int _base = 10) const
	{
		unsigned long long val = 0;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			val = std::stoull(it->second, 0, _base);
			return val;
		}
		return val;
	}

	bool QueryBool(const std::string& AttName) const
	{
		bool val = false;
		auto it = m_setAttrib.find(AttName);
		if(it != m_setAttrib.end() && it->second.empty() == false)
		{
			const std::string& result = it->second;
			val = (result == std::string("TRUE") || result == std::string("true") || result == std::string("1"));
			return val;
		}
		return val;
	}

	void SetVal(const std::string& AttName, const std::string& AttVal)
	{
		m_setAttrib[AttName] = AttVal;
	}
	void SetVal(const std::string& AttName, const char* AttVal)
	{
		if(AttVal == nullptr)
			return;

		m_setAttrib[AttName] = AttVal;
	}
protected:
	std::map<std::string, std::vector<CSettingNode> > m_setChild;
	std::map<std::string, std::string> m_setAttrib;
	friend class CSettingMap;
};
class CSettingMap
{
public:
	CSettingMap() = default;
	

	void Prase(tinyxml2::XMLElement* pRootE)
	{
		if (pRootE == nullptr)
			return;
		PraseChild(m_RootNode, pRootE);
	}
	void PraseAttrib(CSettingNode& node, tinyxml2::XMLElement* pParentE)
	{
		const tinyxml2::XMLAttribute* pAttrib = pParentE->FirstAttribute();
		while (pAttrib)
		{
			std::string val;
			node.SetVal(pAttrib->Name(), pAttrib->Value()? pAttrib->Value() : std::string() );
			pAttrib = pAttrib->Next();
		}
	}
	void PraseChild(CSettingNode& node, tinyxml2::XMLElement* pParentE)
	{
		if (pParentE == nullptr)
			return;
		tinyxml2::XMLElement* pChildE = pParentE->FirstChildElement();
		while(pChildE)
		{
			CSettingNode child_node;
			PraseAttrib(child_node, pChildE);
			PraseChild(child_node, pChildE);
			node[pChildE->Name()].emplace_back(std::move(child_node));
			pChildE = pChildE->NextSiblingElement();
		}
	}


	std::vector<CSettingNode>& operator[](const std::string& name)
	{
		return m_RootNode[name];
	}
	const std::vector<CSettingNode>& operator[](const std::string& name) const
	{
		return m_RootNode[name];
	}
private:
	CSettingNode m_RootNode;
};
#endif // SettingMap_h__

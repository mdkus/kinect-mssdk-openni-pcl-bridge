#pragma once
#include "base.h"
#include "version.h"

template<class ProductionNodeClass>
class ProductionNodeExporter {
private:
	XnPredefinedProductionNodeType m_nodeType;
	const XnChar* m_strName;

public:
	ProductionNodeExporter(const XnChar* strName, XnPredefinedProductionNodeType nodeType)
	{
		m_nodeType = nodeType;
		m_strName = strName;
	}

	void GetDescription(XnProductionNodeDescription* pDescription)
	{
		pDescription->Type = m_nodeType;
		strcpy(pDescription->strVendor, VERSION_VENDOR);
		strcpy(pDescription->strName, m_strName);
		pDescription->Version.nMajor = VERSION_MAJOR;
		pDescription->Version.nMinor = VERSION_MINOR;
		pDescription->Version.nMaintenance = VERSION_MAINTENANCE;
		pDescription->Version.nBuild = VERSION_BUILD;
	}

	XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& nodes, xn::EnumerationErrors* pErrors)
	{
		XnProductionNodeDescription desc;
		GetDescription(&desc);
		return nodes.Add(desc, NULL, NULL);
	}

	XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
	{
		ProductionNodeClass* pGen = new ProductionNodeClass();

		XnStatus nRetVal = pGen->Init(); // ProductionNodeClass must have Init method that returns XnStatus
		if (nRetVal != XN_STATUS_OK) {
			delete pGen;
			return nRetVal;
		}

		*ppInstance = pGen;
	
		return XN_STATUS_OK;
	}

	void Destroy(xn::ModuleProductionNode* pInstance)
	{
		delete pInstance;
	}
};

#define PRODUCTION_NODE_EXPORTER_DEF(ProductionNodeClass, nodeTypeSuffix) \
class Exported##ProductionNodeClass : public ProductionNodeExporter<ProductionNodeClass> \
{ \
public: \
	Exported##ProductionNodeClass() : ProductionNodeExporter(#ProductionNodeClass, XN_NODE_TYPE_##nodeTypeSuffix) {} \
}; \
XN_EXPORT_##nodeTypeSuffix(Exported##ProductionNodeClass)

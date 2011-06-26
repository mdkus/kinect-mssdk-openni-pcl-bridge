#pragma once
#include "base.h"
#include "ProductionNodeExporter.h"

template <class ProductionNodeClass>
class SimpleProductionNodeExporter : public ProductionNodeExporter<ProductionNodeClass>
{
protected:
	SimpleProductionNodeExporter(const XnChar* strName, XnPredefinedProductionNodeType nodeType) : ProductionNodeExporter(strName, nodeType) {}

	virtual XnStatus InitImpl(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
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
};

#define SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(ProductionNodeClass, NodeTypeSuffix) \
class Exported##ProductionNodeClass : public SimpleProductionNodeExporter<ProductionNodeClass> \
{ \
public: \
	Exported##ProductionNodeClass() : SimpleProductionNodeExporter(#ProductionNodeClass, XN_NODE_TYPE_##NodeTypeSuffix) {} \
}; \
XN_EXPORT_##NodeTypeSuffix(Exported##ProductionNodeClass);

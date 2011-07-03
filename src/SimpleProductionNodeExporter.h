#pragma once
#include "base.h"
#include "ProductionNodeExporter.h"

template <class ProductionNodeClass>
class SimpleProductionNodeExporter :
	public ProductionNodeExporter<ProductionNodeClass>
{
protected:
	SimpleProductionNodeExporter(const XnChar* strName, XnPredefinedProductionNodeType nodeType) : ProductionNodeExporter(strName, nodeType) {}

	virtual XnStatus InitImpl(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
	{
		try {
			*ppInstance = new ProductionNodeClass();
			return XN_STATUS_OK;
		} catch (XnStatusException e) {
			return e.nStatus;
		}

	}
};

#define SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(ProductionNodeClass, NodeTypeSuffix) \
class Exported##ProductionNodeClass : public SimpleProductionNodeExporter<ProductionNodeClass> \
{ \
public: \
	Exported##ProductionNodeClass() : SimpleProductionNodeExporter(#ProductionNodeClass, XN_NODE_TYPE_##NodeTypeSuffix) {} \
}; \
XN_EXPORT_##NodeTypeSuffix(Exported##ProductionNodeClass);

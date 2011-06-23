#include "base.h"
#include "MSRKinectDepthGenerator.h"
#include "MSRKinectImageGenerator.h"
#include "ProductionNodeExporter.h"
#include <XnModuleCppRegistratration.h>

XN_EXPORT_MODULE(Module)
PRODUCTION_NODE_EXPORTER_DEF(MSRKinectDepthGenerator, DEPTH);
PRODUCTION_NODE_EXPORTER_DEF(MSRKinectImageGenerator, IMAGE);

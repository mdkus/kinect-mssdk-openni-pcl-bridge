#include "base.h"
#include "MSRKinectUserGenerator.h"
#include "SimpleProductionNodeExporter.h"
#include <XnModuleCppRegistratration.h>

XN_EXPORT_MODULE(Module)

//#include "MSRKinectDepthGenerator.h"
//SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinectDepthGenerator, DEPTH);

#include "MSRKinectDepthGeneratorCompatibleWithPrimeSense.h"
SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinectDepthGeneratorCompatibleWithPrimeSense, DEPTH);

#include "MSRKinectImageGenerator.h"
SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinectImageGenerator, IMAGE);

#include "MSRKinectUserSkeletonGenerator.h"
SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinectUserSkeletonGenerator, USER);

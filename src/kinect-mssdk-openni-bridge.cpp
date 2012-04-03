#include "base.h"
#include "MSRKinectUserGenerator.h"
#include "SimpleProductionNodeExporter.h"
#include <XnModuleCppRegistratration.h>

XN_EXPORT_MODULE(Module)

#if KINECTSDK_VER >= 100
#define DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(ProductionNodeClassBody, NodeTypeSuffix) \
	typedef MSRKinect##ProductionNodeClassBody KinectSDK##ProductionNodeClassBody; \
	SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(KinectSDK##ProductionNodeClassBody, NodeTypeSuffix);
#else
#define DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(ProductionNodeClassBody, NodeTypeSuffix) \
	SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinect##ProductionNodeClassBody, NodeTypeSuffix);
#endif

//#include "MSRKinectDepthGenerator.h"
//SIMPLE_PRODUCTION_NODE_EXPORTER_DEF(MSRKinectDepthGenerator, DEPTH);

#include "MSRKinectDepthGeneratorCompatibleWithPrimeSense.h"
DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(DepthGeneratorCompatibleWithPrimeSense, DEPTH);

#include "MSRKinectImageGenerator.h"
DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(ImageGenerator, IMAGE);

#include "MSRKinectUserSkeletonGenerator.h"
DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(UserSkeletonGenerator, USER);

#include "MSRKinectUserSkeletonGeneratorWithPsiPoseEmulation.h"
DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(UserSkeletonGeneratorWithPsiPoseEmulation, USER);

#include "MSRKinectUserSkeletonGeneratorWithAutoElevation.h"
DEFINE_PRODUCTION_NODE_EXPORTER_DEFS(UserSkeletonGeneratorWithAutoElevation, USER);

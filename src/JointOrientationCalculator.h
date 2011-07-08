#pragma once
#include "base.h"
#include "MSRKinectJointMap.h"

XnVector3D POS_X = {1.0f, 0, 0};
XnVector3D POS_Y = {0, 1.0f, 0};
XnVector3D POS_Z = {0, 0, 1.0f};
XnVector3D NEG_X = {-1.0f, 0, 0};
XnVector3D NEG_Y = {0, -1.0f, 0};
XnVector3D NEG_Z = {0, 0, -1.0f};

class JointOrientationCalculator
{
public:
	xn::ModuleSkeletonInterface* m_pSkeleton;

	JointOrientationCalculator(xn::ModuleSkeletonInterface* pSkeleton) : m_pSkeleton(pSkeleton)
	{
	}

	typedef enum _VECTOR_DIRECTION {
		VECTOR_DIRECTION_POS_X = 0,
		VECTOR_DIRECTION_POS_Y,
		VECTOR_DIRECTION_POS_Z,
		VECTOR_DIRECTION_NEG_X,
		VECTOR_DIRECTION_NEG_Y,
		VECTOR_DIRECTION_NEG_Z
	} VECTOR_DIRECTION;

	XnFloat dotProduct(XnVector3D u, XnVector3D v) {
		return u.X*v.X + u.Y*v.Y + u.Z*v.Z;
	}

	bool areNearCollinear (XnVector3D v1, XnVector3D v2) {
		XnFloat product = dotProduct(GetNormalizedVector(v1),GetNormalizedVector(v2));
		XnFloat minProduct = 0.8f;
		return product >= minProduct;
	}

	XnVector3D crossProduct(XnVector3D u, XnVector3D v) {
		XnVector3D w;
		w.X = u.Y*v.Z - u.Z*v.Y;
		w.Y = u.Z*v.X - u.X*v.Z;
		w.Z = u.X*v.Y - u.Y*v.X;
		return w;
	}

	//returns a normalized version of the supplied vector
	XnVector3D GetNormalizedVector(XnVector3D v) {
		XnVector3D vnew;
		XnFloat oneovernorm = 1.0f/sqrt(v.X*v.X+v.Y*v.Y+v.Z*v.Z);
		vnew.X = v.X * oneovernorm;
		vnew.Y = v.Y * oneovernorm;
		vnew.Z = v.Z * oneovernorm;
		return vnew;
	}

	//returns the negative of the supplied vector 
	XnVector3D GetNegativeVector(XnVector3D v) {
		XnVector3D nv;
		nv.X = - v.X;
		nv.Y = - v.Y;
		nv.Z = - v.Z;
		return nv;
	}

	XnVector3D GetRelativePosition(XnVector3D start, XnVector3D end) {
		XnVector3D diff;
		diff.X = end.X - start.X;
		diff.Y = end.Y - start.Y;
		diff.Z = end.Z - start.Z;
		return diff;
	}

	XnVector3D GetPositionBetweenIndices(XnUserID user, NUI_SKELETON_POSITION_INDEX p1, NUI_SKELETON_POSITION_INDEX p2) {
		
		XnSkeletonJoint joint1 = MSRKinectJointMap::GetXnJointByNuiIndex(p1);
		XnSkeletonJoint joint2 = MSRKinectJointMap::GetXnJointByNuiIndex(p2);
		
		XnSkeletonJointPosition pos1; 
		XnSkeletonJointPosition pos2;
		m_pSkeleton->GetSkeletonJointPosition( user, joint1, pos1 );
		m_pSkeleton->GetSkeletonJointPosition( user, joint2, pos2 );
		XnVector3D pVec1 = pos1.position;
		XnVector3D pVec2 = pos2.position;

		return GetRelativePosition( pVec1, pVec2 );
	}
		
	//populate matrix using the columns
	void PopulateMatrix(XnSkeletonJointOrientation &jointOrientation, XnVector3D xCol,XnVector3D yCol, XnVector3D zCol) {
		
		jointOrientation.orientation.elements[0] = xCol.X;
		jointOrientation.orientation.elements[3] = xCol.Y;
		jointOrientation.orientation.elements[6] = xCol.Z;

		jointOrientation.orientation.elements[1] = yCol.X;
		jointOrientation.orientation.elements[4] = yCol.Y;
		jointOrientation.orientation.elements[7] = yCol.Z;

		jointOrientation.orientation.elements[2] = zCol.X;
		jointOrientation.orientation.elements[5] = zCol.Y;
		jointOrientation.orientation.elements[8] = zCol.Z;
	}

	//constructs an orientation from a vector that specifies the x axis
	void MakeMatrixFromX(XnVector3D v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		//get normalized version of vector between joints
		XnVector3D v1normalized = GetNormalizedVector(v1);

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		xCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		yCol.X = 0.0f;
		yCol.Y = xCol.Z;
		yCol.Z = -xCol.Y;

		yCol = GetNormalizedVector(yCol);

		//third column is fully determined by the first two, and it must be their cross product
		zCol = crossProduct(xCol,yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}

	//constructs an orientation from a vector that specifies the y axis
	void MakeMatrixFromY(XnVector3D v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		//get normalized version of vector between joints
		XnVector3D v1normalized = GetNormalizedVector(v1);

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		yCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		xCol.X = yCol.Y;
		xCol.Y = -yCol.X;
		xCol.Z = 0.0f;

		xCol = GetNormalizedVector(xCol);

		//third column is fully determined by the first two, and it must be their cross product
		zCol = crossProduct(xCol,yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}
	
	//constructs an orientation from a vector that specifies the x axis
	void MakeMatrixFromZ(XnVector3D v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		//get normalized version of vector between joints
		XnVector3D v1normalized = GetNormalizedVector(v1);

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		zCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		xCol.X = zCol.Y;
		xCol.Y = -zCol.X;
		xCol.Z = 0.0f;

		xCol = GetNormalizedVector(xCol);

		//third column is fully determined by the first two, and it must be their cross product
		yCol = crossProduct(zCol,xCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}

	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the first vector as x axis, then constructs the other axes using cross products
	void MakeMatrixFromXY(XnVector3D x_unnormalized, XnVector3D y_unnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		XnVector3D xn = GetNormalizedVector(x_unnormalized);
		XnVector3D yn = GetNormalizedVector(y_unnormalized);

		//set up the three different columns to be rearranged and flipped
		xCol = xn;
		zCol = GetNormalizedVector(crossProduct(xCol,yn));
		yCol = crossProduct(zCol,xCol);//shouldn't need normalization

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}
	
	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the second vector as y axis, then constructs the other axes using cross products
	void MakeMatrixFromYX(XnVector3D x_unnormalized, XnVector3D y_unnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		XnVector3D xn = GetNormalizedVector(x_unnormalized);
		XnVector3D yn = GetNormalizedVector(y_unnormalized);

		//set up the three different columns to be rearranged and flipped
		yCol = yn;
		zCol = GetNormalizedVector(crossProduct(xn,yCol));
		xCol = crossProduct(yCol,zCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}
	
	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the second vector as y axis, then constructs the other axes using cross products
	void MakeMatrixFromYZ(XnVector3D y_unnormalized, XnVector3D z_unnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XnVector3D xCol;
		XnVector3D yCol;
		XnVector3D zCol;

		XnVector3D yn = GetNormalizedVector(y_unnormalized);
		XnVector3D zn = GetNormalizedVector(z_unnormalized);

		//set up the three different columns to be rearranged and flipped
		yCol = yn;
		xCol = GetNormalizedVector(crossProduct(yCol,zn));
		zCol = crossProduct(xCol,yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation,xCol,yCol,zCol);
	}

	virtual XnStatus GetSkeletonJointOrientation(XnUserID user, NUI_SKELETON_POSITION_INDEX index, XnSkeletonJointOrientation& jointOrientation)
	{
		XnVector3D vx;
		XnVector3D vy;
		XnVector3D vz;

		switch (index) {
		
		case NUI_SKELETON_POSITION_HIP_CENTER://use hipcenter-spine for y, hipleft-hipright for x
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_CENTER,NUI_SKELETON_POSITION_SPINE);
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
			MakeMatrixFromYX(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_SPINE://use shoulderleft-shoulderright for x
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SPINE,NUI_SKELETON_POSITION_SHOULDER_CENTER);
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_SHOULDER_RIGHT);
			MakeMatrixFromYX(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_SHOULDER_CENTER://use shoulderleft-shoulder-right for x
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_CENTER,NUI_SKELETON_POSITION_HEAD);
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_SHOULDER_RIGHT);
			MakeMatrixFromYX(vx,vy,jointOrientation);
			break;

		case NUI_SKELETON_POSITION_HEAD:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_CENTER,NUI_SKELETON_POSITION_HEAD);
			MakeMatrixFromY(vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_SHOULDER_LEFT:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_LEFT,NUI_SKELETON_POSITION_WRIST_LEFT);
			vx = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT));
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_ELBOW_LEFT:
			vx = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_LEFT,NUI_SKELETON_POSITION_WRIST_LEFT));
			vy = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT));
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_WRIST_LEFT:
			vx = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT));
			MakeMatrixFromX(vx,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_HAND_LEFT:
			vx = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT));
			MakeMatrixFromX(vx,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_HIP_LEFT:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_HIP_LEFT);
			//vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);//this one could be off
			//if(areNearCollinear(vy,vz)) {
				vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
				MakeMatrixFromYX(vx,vy,jointOrientation);
			//}else{
			//	MakeMatrixFromYZ(vy,vz,jointOrientation);
			//}
			break;
			
		case NUI_SKELETON_POSITION_KNEE_LEFT:
			{
				vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
				vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_LEFT,NUI_SKELETON_POSITION_FOOT_LEFT);//this one could be off
				XnSkeletonJointPosition footPos;
				XnSkeletonJoint footJoint;
				footJoint = MSRKinectJointMap::GetXnJointByNuiIndex(NUI_SKELETON_POSITION_FOOT_LEFT);
				m_pSkeleton->GetSkeletonJointPosition(user,footJoint,footPos);
				XnFloat minConfidence = 0.9f;
				if (footPos.fConfidence < minConfidence) {
					printf("Knee position confidence was: %f\r\n",footPos.fConfidence);
					MakeMatrixFromY(vy,jointOrientation);
				}else {
					MakeMatrixFromYZ(vy,vz,jointOrientation);
				}
			}
			break;
			
		case NUI_SKELETON_POSITION_ANKLE_LEFT:
			vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
			MakeMatrixFromZ(vz,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_FOOT_LEFT:
			vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
			MakeMatrixFromZ(vz,jointOrientation);
			break;
			
			
		case NUI_SKELETON_POSITION_SHOULDER_RIGHT:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_RIGHT,NUI_SKELETON_POSITION_WRIST_RIGHT);
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_RIGHT,NUI_SKELETON_POSITION_ELBOW_RIGHT);
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_ELBOW_RIGHT:
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_RIGHT,NUI_SKELETON_POSITION_WRIST_RIGHT);
			vy = GetNegativeVector(GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_RIGHT,NUI_SKELETON_POSITION_ELBOW_RIGHT));
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_WRIST_RIGHT:
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_RIGHT,NUI_SKELETON_POSITION_HAND_RIGHT);
			MakeMatrixFromX(vx,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_HAND_RIGHT:
			vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_RIGHT,NUI_SKELETON_POSITION_HAND_RIGHT);
			MakeMatrixFromX(vx,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_HIP_RIGHT:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_HIP_RIGHT);
			//vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);//this one could be off
			//if(areNearCollinear(vy,vz)) {
				vx = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_HIP_LEFT,NUI_SKELETON_POSITION_HIP_RIGHT);
				MakeMatrixFromYX(vx,vy,jointOrientation);
			//}else{
			//	MakeMatrixFromYZ(vy,vz,jointOrientation);
			//}
			break;
			
		case NUI_SKELETON_POSITION_KNEE_RIGHT:
			vy = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
			vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_RIGHT,NUI_SKELETON_POSITION_FOOT_RIGHT);//this one could be off
			MakeMatrixFromYZ(vy,vz,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_ANKLE_RIGHT:
			vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
			MakeMatrixFromZ(vz,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_FOOT_RIGHT:
			vz = GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_FOOT_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
			MakeMatrixFromZ(vz,jointOrientation);
			break;
			
		}
		return XN_STATUS_OK;
	}
};

class JointOrientationCalculatorTest
{
public:
	void test()
	{
	}
};
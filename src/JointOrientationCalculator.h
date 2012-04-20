//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "vec.h"
#include "MSRKinectJointMap.h"

// "Guess" the orientation of the joints. Thanks to my friend, Mr. Contributor!
class JointOrientationCalculator
{
private:
	xn::ModuleSkeletonInterface* m_pSkeleton;

public:
	JointOrientationCalculator(xn::ModuleSkeletonInterface* pSkeleton) : m_pSkeleton(pSkeleton)
	{
	}

	bool areNearCollinear (const XV3& v1, XV3& v2) {
		XnFloat product = v1.dotNormalized(v2);
		XnFloat minProduct = 0.8f;
		return product >= minProduct;
	}

	XV3 GetPositionBetweenIndices(XnUserID user, NUI_SKELETON_POSITION_INDEX p1, NUI_SKELETON_POSITION_INDEX p2) {
		
		XnSkeletonJoint joint1 = MSRKinectJointMap::GetXnJointByNuiIndex(p1);
		XnSkeletonJoint joint2 = MSRKinectJointMap::GetXnJointByNuiIndex(p2);
		
		XnSkeletonJointPosition pos1; 
		XnSkeletonJointPosition pos2;
		m_pSkeleton->GetSkeletonJointPosition( user, joint1, pos1 );
		m_pSkeleton->GetSkeletonJointPosition( user, joint2, pos2 );
		XV3 pVec1 = pos1.position;
		XV3 pVec2 = pos2.position;

		return pVec2 - pVec1;
	}
		
	//populate matrix using the columns
	void PopulateMatrix(XnSkeletonJointOrientation &jointOrientation, const XnVector3D& xCol,const XnVector3D& yCol, const XnVector3D& zCol) {
		
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
	void MakeMatrixFromX(const XV3& v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		//get normalized version of vector between joints
		XV3 v1normalized = v1.normalize();

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		xCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		yCol.X = 0.0f;
		yCol.Y = xCol.Z;
		yCol.Z = -xCol.Y;

		yCol.normalizeM();

		//third column is fully determined by the first two, and it must be their cross product
		zCol = xCol.cross(yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
	}

	//constructs an orientation from a vector that specifies the y axis
	void MakeMatrixFromY(const XV3& v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		//get normalized version of vector between joints
		XnVector3D v1normalized = v1.normalize();

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		yCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		xCol.X = yCol.Y;
		xCol.Y = -yCol.X;
		xCol.Z = 0.0f;

		xCol.normalizeM();

		//third column is fully determined by the first two, and it must be their cross product
		zCol = xCol.cross(yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
	}
	
	//constructs an orientation from a vector that specifies the x axis
	void MakeMatrixFromZ(const XV3& v1, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		//get normalized version of vector between joints
		XV3 v1normalized = v1.normalize();

		//set first column to the vector between the previous joint and the current one, this sets the two degrees of freedom
		zCol = v1normalized;

		//set second column to an arbitrary vector perpendicular to the first column
		//the angle can be arbitrary since it corresponds to the rotation about relative position vector, which doesn't matter
		xCol.X = zCol.Y;
		xCol.Y = -zCol.X;
		xCol.Z = 0.0f;

		xCol.normalizeM();

		//third column is fully determined by the first two, and it must be their cross product
		yCol = zCol.cross(xCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
	}

	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the first vector as x axis, then constructs the other axes using cross products
	void MakeMatrixFromXY(const XV3& xUnnormalized, const XV3& yUnnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		XV3 xn = xUnnormalized.normalize();
		XV3 yn = yUnnormalized.normalize();

		//set up the three different columns to be rearranged and flipped
		xCol = xn;
		zCol = xCol.cross(yn).normalize();
		yCol = zCol.cross(xCol); //shouldn't need normalization

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
	}
	
	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the second vector as y axis, then constructs the other axes using cross products
	void MakeMatrixFromYX(const XV3& xUnnormalized, const XV3& yUnnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		XV3 xn = xUnnormalized.normalize();
		XV3 yn = yUnnormalized.normalize();

		//set up the three different columns to be rearranged and flipped
		yCol = yn;
		zCol = xn.cross(yCol).normalize();
		xCol = yCol.cross(zCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
	}
	
	//constructs an orientation from 2 vectors: the first specifies the x axis, and the next specifies the y axis
	//uses the second vector as y axis, then constructs the other axes using cross products
	void MakeMatrixFromYZ(const XV3& yUnnormalized, const XV3& zUnnormalized, XnSkeletonJointOrientation &jointOrientation) {
		
		//matrix columns
		XV3 xCol;
		XV3 yCol;
		XV3 zCol;

		XV3 yn = yUnnormalized.normalize();
		XV3 zn = zUnnormalized.normalize();

		//set up the three different columns to be rearranged and flipped
		yCol = yn;
		xCol = yCol.cross(zn).normalize();
		zCol = xCol.cross(yCol);

		//copy values into matrix
		PopulateMatrix(jointOrientation, xCol, yCol, zCol);
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
			vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT);
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_ELBOW_LEFT:
			vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ELBOW_LEFT,NUI_SKELETON_POSITION_WRIST_LEFT);
			vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_LEFT,NUI_SKELETON_POSITION_ELBOW_LEFT);
			MakeMatrixFromXY(vx,vy,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_WRIST_LEFT:
			vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT);
			MakeMatrixFromX(vx,jointOrientation);
			break;
			
		case NUI_SKELETON_POSITION_HAND_LEFT:
			vx = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_WRIST_LEFT,NUI_SKELETON_POSITION_HAND_LEFT);
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
				vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_LEFT,NUI_SKELETON_POSITION_ANKLE_LEFT);
				vz = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_LEFT,NUI_SKELETON_POSITION_FOOT_LEFT);//this one could be off
				XnSkeletonJointPosition footPos;
				XnSkeletonJoint footJoint;
				footJoint = MSRKinectJointMap::GetXnJointByNuiIndex(NUI_SKELETON_POSITION_FOOT_LEFT);
				m_pSkeleton->GetSkeletonJointPosition(user,footJoint,footPos);
				XnFloat minConfidence = 0.9f;
				if (footPos.fConfidence < minConfidence) {
					//printf("Knee position confidence was: %f\r\n",footPos.fConfidence);
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
			vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_SHOULDER_RIGHT,NUI_SKELETON_POSITION_ELBOW_RIGHT);
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
			vy = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_KNEE_RIGHT,NUI_SKELETON_POSITION_ANKLE_RIGHT);
			vz = -GetPositionBetweenIndices(user,NUI_SKELETON_POSITION_ANKLE_RIGHT,NUI_SKELETON_POSITION_FOOT_RIGHT);//this one could be off
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
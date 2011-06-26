#pragma once
#include "base.h"
#include "AbstractMSRKinectImageStreamGenerator.h"
#include "MSRKinectGeneratorControls.h"

class MSRKinectUserGenerator :
	public virtual AbstractMSRKinectImageStreamGenerator<xn::ModuleUserGenerator, USHORT, XnLabel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX>
{
private:
	typedef AbstractMSRKinectImageStreamGenerator<xn::ModuleUserGenerator, USHORT, XnLabel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX> SuperClass;

	struct UserCallbackHandleSet {
		XnCallbackHandle hNewUserCallbackHandle;
		XnCallbackHandle hLostUserCallbackHandle;

		UserCallbackHandleSet() : hNewUserCallbackHandle(NULL), hLostUserCallbackHandle(NULL) {}
	};
	typedef std::vector<UserCallbackHandleSet*> UserCallbackHandleSetList;

	XN_DECLARE_EVENT_1ARG(UserEvent, UserEventInterface, XnUserID, user);

private:
	static const int MAX_USERS = 8;

	XnUInt16 m_nNumberOfUsers;
	XnUInt8 m_nUsersMask;
	XnLabel* m_pFilteredBuffer;

	UserCallbackHandleSetList m_userCallbackHandleSets;

	UserEvent m_newUserEvent;
	UserEvent m_lostUserEvent;

public:
	MSRKinectUserGenerator() : m_nNumberOfUsers(0), m_nUsersMask(0), m_pFilteredBuffer(NULL)
	{
	}

	virtual ~MSRKinectUserGenerator()
	{
		if (m_pFilteredBuffer) delete [] m_pFilteredBuffer;

		for (UserCallbackHandleSetList::iterator i = m_userCallbackHandleSets.begin(); i != m_userCallbackHandleSets.end(); i++) {
			delete *i;
		}
	}

	virtual const void* GetData()
	{
		return m_pBuffer;
	}

	virtual XnUInt16 GetNumberOfUsers()
	{
		return m_nNumberOfUsers;
	}

	virtual XnStatus GetUsers(XnUserID* pUsers, XnUInt16& nUsers)
	{
		if (nUsers == 0) {
			return XN_STATUS_OK;
		}

		XnUInt16 usersMask = m_nUsersMask;
		XnUserID userID = 1;
		XnUserID actualNumberOfUsers = 0;

		while (usersMask && actualNumberOfUsers < nUsers) {
			if (usersMask & 1) {
				pUsers[actualNumberOfUsers++] = userID;
			}
			usersMask >>= 1;
			userID++;
		}
		nUsers = actualNumberOfUsers;

		return XN_STATUS_OK;
	}

	virtual XnStatus GetCoM(XnUserID user, XnPoint3D& com) {
		// Does not work without skeleton tracking
		return XN_STATUS_OK;
	}

	virtual XnStatus GetUserPixels(XnUserID user, XnSceneMetaData *pScene)
	{
		if (user == 0) {
			pScene->pData = m_pBuffer;
		} else {
			if (!m_pFilteredBuffer) {
				m_pFilteredBuffer = new XnLabel[X_RES * Y_RES];
			}
			for (int i = 0; i < X_RES * Y_RES; i++) {
				m_pFilteredBuffer[i] = m_pBuffer[i] == user ? user : 0;
			}
			pScene->pData = m_pFilteredBuffer;
		}
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterUserCallbacks(XnModuleUserHandler NewUserCB, XnModuleUserHandler LostUserCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		UserCallbackHandleSet* pCallbackHandleSet = new UserCallbackHandleSet();
		if (NewUserCB) m_newUserEvent.Register(NewUserCB, pCookie, &pCallbackHandleSet->hNewUserCallbackHandle);
		if (LostUserCB) m_lostUserEvent.Register(LostUserCB, pCookie, &pCallbackHandleSet->hLostUserCallbackHandle);
		m_userCallbackHandleSets.push_back(pCallbackHandleSet);
		hCallback = pCallbackHandleSet;
		return XN_STATUS_OK;
	}

	virtual void UnregisterUserCallbacks(XnCallbackHandle hCallback)
	{
		UserCallbackHandleSetList::iterator i = 
			std::find(m_userCallbackHandleSets.begin(), m_userCallbackHandleSets.end(), hCallback);
		if (i != m_userCallbackHandleSets.end()) {
			m_newUserEvent.Unregister((*i)->hNewUserCallbackHandle);
			m_lostUserEvent.Unregister((*i)->hLostUserCallbackHandle);
			delete *i;
			m_userCallbackHandleSets.erase(i);
		}
	}

	PassiveMSRKinectGeneratorControl_IMPL(m_pReader);

protected:
	inline void processPixel(const USHORT* sp, XnLabel* dp)
	{
		XnLabel label = *sp & NUI_IMAGE_PLAYER_INDEX_MASK;
		*dp = *(dp+1) = *(dp+640) = *(dp+641) = label;
		if (label) {
			m_nUsersMask |= 1 << (label-1);
		}
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
	{
		// todo flexible resolution

		assert(lockedRect.Pitch == 320 * sizeof(USHORT));

		XnUInt16 previousUsersMask = m_nUsersMask;
		m_nUsersMask = 0;

		const USHORT* sp = data;
		XnLabel* dp = m_pBuffer;

		if (!m_pReader->IsCalibrateViewPoint()) {
			for (XnUInt y = 0; y < 240; y++) {
				for (XnUInt x = 0; x < 320; x++) {
					processPixel(sp, dp);
					sp++;
					dp += 2;
				}
				dp += 640;
			}
		} else {
			memset(m_pBuffer, 0, 640*480);

			for (int y = 0; y < 240; y++) {
				for (int x = 0; x < 320; x++) {
					LONG ix, iy;
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, *sp &  ~NUI_IMAGE_PLAYER_INDEX_MASK, &ix, &iy);
					if (ix >= 0 && ix < 639 && iy >= 0 && iy < 479) {
						processPixel(sp, m_pBuffer + iy * 640 + ix);
					}
					sp++;
				}
			}
		}

		m_nNumberOfUsers = countBits(m_nUsersMask);

		XnUInt16 lostUsersMask = previousUsersMask & ~m_nUsersMask;
		XnUserID lostUserID = 1;
		while (lostUsersMask) {
			if (lostUsersMask & 1) {
				m_lostUserEvent.Raise(lostUserID);
			}
			lostUserID++;
			lostUsersMask >>= 1;
		}

		XnUInt16 newUsersMask = ~previousUsersMask & m_nUsersMask;
		XnUserID newUserID = 1;
		while (newUsersMask) {
			if (newUsersMask & 1) {
				m_newUserEvent.Raise(newUserID);
			}
			newUserID++;
			newUsersMask >>= 1;
		}

		return XN_STATUS_OK;
	}

private:
	static XnUInt countBits(XnUInt value)
	{
		XnUInt result = 0;
		while (value) {
			if (value & 1) result++;
			value >>= 1;
		}
		return result;
	}
};

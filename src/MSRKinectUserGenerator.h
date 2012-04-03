#pragma once
#include "base.h"
#include "MSRKinectDepthGeneratorBase.h"

class MSRKinectUserGeneratorDepthPixelProcessor
{
public:
	XnUInt16 nUsersMask;
	MSRKinectUserGeneratorDepthPixelProcessor() : nUsersMask(0) {}

	void Process(USHORT d, XnDepthPixel* dp, XnUInt32 ratio)
	{
		XnLabel label = d & NUI_IMAGE_PLAYER_INDEX_MASK;
		*dp = label;
		if (ratio == 2) {
			*(dp+1) = *(dp+640) = *(dp+641) = label;
		}
		if (label) {
			nUsersMask |= 1 << (label-1);
		}
	}
};

class MSRKinectUserGenerator :
	public MSRKinectDepthGeneratorBase<xn::ModuleUserGenerator, MSRKinectUserGeneratorDepthPixelProcessor>
{
private:
	typedef MSRKinectDepthGeneratorBase SuperClass;

	struct UserCallbackHandleSet {
		XnCallbackHandle hNewUser;
		XnCallbackHandle hLostUser;

		UserCallbackHandleSet() : hNewUser(NULL), hLostUser(NULL) {}
	};
	typedef std::vector<UserCallbackHandleSet*> UserCallbackHandleSetList;

	XN_DECLARE_EVENT_1ARG(UserEvent, UserEventInterface, XnUserID, user);

private:
	static const int MAX_USERS = 8;

	XnUInt16 m_nNumberOfUsers;
	XnUInt16 nUsersMask;
	XnLabel* m_pFilteredBuffer;

	UserCallbackHandleSetList m_userCallbackHandleSets;

	UserEvent m_newUserEvent;
	UserEvent m_lostUserEvent;

public:
	MSRKinectUserGenerator() :
		MSRKinectDepthGeneratorBase(XN_NODE_TYPE_USER, FALSE),
		m_nNumberOfUsers(0), nUsersMask(0), m_pFilteredBuffer(NULL)
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

		XnUInt16 usersMask = nUsersMask;
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
		PopulateMapMetaData(pScene->pMap);
		setupBuffer();
		if (user == 0) {
			pScene->pData = m_pBuffer;
		} else {
			XnUInt32 dataSize = GetXRes() * GetYRes();
			if (!m_pFilteredBuffer) {
				m_pFilteredBuffer = new XnLabel[dataSize];
			}
			for (XnUInt32 i = 0; i < dataSize; i++) {
				m_pFilteredBuffer[i] = (m_pBuffer[i] == user) ? user : 0;
			}
			pScene->pData = m_pFilteredBuffer;
		}
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterUserCallbacks(XnModuleUserHandler NewUserCB, XnModuleUserHandler LostUserCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		UserCallbackHandleSet* pCallbackHandleSet = new UserCallbackHandleSet();
		if (NewUserCB) m_newUserEvent.Register(NewUserCB, pCookie, &pCallbackHandleSet->hNewUser);
		if (LostUserCB) m_lostUserEvent.Register(LostUserCB, pCookie, &pCallbackHandleSet->hLostUser);
		m_userCallbackHandleSets.push_back(pCallbackHandleSet);
		hCallback = pCallbackHandleSet;
		return XN_STATUS_OK;
	}

	virtual void UnregisterUserCallbacks(XnCallbackHandle hCallback)
	{
		UserCallbackHandleSetList::iterator i = 
			std::find(m_userCallbackHandleSets.begin(), m_userCallbackHandleSets.end(), hCallback);
		if (i != m_userCallbackHandleSets.end()) {
			m_newUserEvent.Unregister((*i)->hNewUser);
			m_lostUserEvent.Unregister((*i)->hLostUser);
			delete *i;
			m_userCallbackHandleSets.erase(i);
		}
	}

	XnStatus RegisterToUserExit(XnModuleUserHandler UserExitCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromUserExit(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

	XnStatus RegisterToUserReEnter(XnModuleUserHandler UserReEnterCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromUserReEnter(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

protected:

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		XnUInt32 previousUsersMask = nUsersMask;

		MSRKinectUserGeneratorDepthPixelProcessor proc;
		UpdateDepthData(proc, pFrame, data, lockedRect);
		nUsersMask = proc.nUsersMask;
		m_nNumberOfUsers = CountBits(nUsersMask);

		CheckLostUsers(previousUsersMask & ~nUsersMask);
		CheckNewUsers(~previousUsersMask & nUsersMask);
		return XN_STATUS_OK;
	}

	// callbacks
	virtual void OnNewUser(XnUserID userID)
	{
		m_newUserEvent.Raise(userID);
	}

	virtual void OnLostUser(XnUserID userID)
	{
		m_lostUserEvent.Raise(userID);
	}

private:
	static XnUInt CountBits(XnUInt32 value)
	{
		XnUInt32 result = 0;
		while (value) {
			if (value & 1) result++;
			value >>= 1;
		}
		return result;
	}

	void CheckLostUsers(XnUInt16 mask)
	{
		XnUserID userID = 1;
		while (mask) {
			if (mask & 1) {
				OnLostUser(userID);
			}
			userID++;
			mask >>= 1;
		}
	}

	void CheckNewUsers(XnUInt16 mask)
	{
		XnUserID userID = 1;
		while (mask) {
			if (mask & 1) {
				OnNewUser(userID);
			}
			userID++;
			mask >>= 1;
		}
	}
};

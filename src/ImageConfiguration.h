#pragma once
#include "base.h"

class ImageConfiguration
{
public:
	class Mode
	{
	public:
		XnMapOutputMode outputMode;
		NUI_IMAGE_RESOLUTION eResolution;

		Mode(XnUInt32 nXRes, XnUInt32 nYRes, XnUInt32 nFPS, NUI_IMAGE_RESOLUTION eRes)
		{
			outputMode.nXRes = nXRes;
			outputMode.nYRes = nYRes;
			outputMode.nFPS = nFPS;
			eResolution = eRes;
		}
	};

	class Desc
	{
	public:
		NUI_IMAGE_TYPE eImageType;
		const Mode* supportedModes;
		XnUInt32 numberOfSupportedModes;

		Desc(NUI_IMAGE_TYPE _eImageType, const Mode* _supportedModes, XnUInt32 _numberOfSupportedModes) :
			eImageType(_eImageType), supportedModes(_supportedModes), numberOfSupportedModes(_numberOfSupportedModes) {}
	};

protected:
	const Desc* m_pDesc;
	XnUInt32 m_selectedModeIndex;

public:
	NUI_IMAGE_TYPE GetImageType() const { return m_pDesc->eImageType; }
	const Mode* GetSupportedModes() const { return m_pDesc->supportedModes; }
	XnUInt32 GetNumberOfSupportedModes() const { return m_pDesc->numberOfSupportedModes; }

	XnUInt32 GetSelectedModeIndex() const { return m_selectedModeIndex; }
	void SetSelectedModeIndex(XnUInt32 index) { m_selectedModeIndex = index; }
	const Mode* GetSelectedMode() const { return GetSupportedModeAt(m_selectedModeIndex); }
	const Mode* GetSupportedModeAt(XnUInt32 index) const { return GetSupportedModes() + index; }

	void SetDesc(const Desc* pDesc) { m_pDesc = pDesc; }

public:
	ImageConfiguration(const Desc* pDesc = NULL) : m_pDesc(pDesc), m_selectedModeIndex(0) {}

	XnStatus Select(const XnMapOutputMode& mode)
	{
		for (XnUInt32 i = 0; i < GetNumberOfSupportedModes(); i++) {
			// check resolution. ignore FPS.
			if (GetSupportedModeAt(i)->outputMode.nXRes == mode.nXRes &&
					GetSupportedModeAt(i)->outputMode.nYRes == mode.nYRes) {
				SetSelectedModeIndex(i);
				return XN_STATUS_OK;
			}
		}
		return XN_STATUS_BAD_PARAM;
	}
};

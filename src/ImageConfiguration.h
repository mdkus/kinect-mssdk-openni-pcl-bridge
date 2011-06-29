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

protected:
	NUI_IMAGE_TYPE m_eImageType;
	Mode* m_supportedModes;
	XnUInt32 m_numberOfSuppotedModes;
	XnUInt32 m_selectedModeIndex;

public:
	virtual ~ImageConfiguration() {}

	NUI_IMAGE_TYPE GetImageType() const { return m_eImageType; }
	const Mode* GetSupportedModes() const { return m_supportedModes; }
	XnUInt32 GetNumberOfSupportedModes() const { return m_numberOfSuppotedModes; }

	XnUInt32 GetSelectedModeIndex() const { return m_selectedModeIndex; }
	void SetSelectedModeIndex(XnUInt32 index) { m_selectedModeIndex = index; }
	const Mode* GetSelectedMode() const { return GetSupportedModeAt(m_selectedModeIndex); }
	const Mode* GetSupportedModeAt(XnUInt32 index) const { return m_supportedModes + index; }

protected:
	ImageConfiguration(NUI_IMAGE_TYPE eImageType) : m_selectedModeIndex(0), m_eImageType(eImageType) {}

	void Init(Mode* supportedModes, XnUInt32 numberOfSupportedModes)
	{
		m_supportedModes = supportedModes;
		m_numberOfSuppotedModes = numberOfSupportedModes;
	}

public:
	XnStatus Select(const XnMapOutputMode& mode)
	{
		for (XnUInt32 i = 0; i < m_numberOfSuppotedModes; i++) {
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

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
#include "AbstractModuleGenerator.h"
#include "IMultiThreadFrameReader.h"

template <class ParentModuleGeneratorClass, class FrameReaderClass>
class AbstractMultiThreadFrameGenerator :
	public AbstractModuleGenerator<ParentModuleGeneratorClass>,
	public virtual IMultiThreadFrameReader::IListener
{
private:
	typedef AbstractModuleGenerator<ParentModuleGeneratorClass> SuperClass;

protected:
	FrameReaderClass* m_pReader;
	BOOL m_bActiveGeneratorControl;
	BOOL m_bNewDataAvailable;

public:
	AbstractMultiThreadFrameGenerator(BOOL bActiveGeneratorControl) :
		m_bActiveGeneratorControl(bActiveGeneratorControl),
		m_bNewDataAvailable(FALSE)
	{
	}

	virtual ~AbstractMultiThreadFrameGenerator()
	{
		if (m_pReader) {
			m_pReader->RemoveListener(this);
		}
	}

	// ReaderClass::Listener methods

	virtual void OnUpdateFrame()
	{
		m_bNewDataAvailable = TRUE;
		m_newDataAvailableEvent.Raise();
	}

	virtual void OnStartReadingFrame()
	{
		m_generationRunningChangeEvent.Raise();
	}

	virtual void OnStopReadingFrame()
	{
		m_generationRunningChangeEvent.Raise();
	}

	//
	// Generator methods
	//

	virtual XnBool IsNewDataAvailable(XnUInt64& nTimestamp)
	{
		return m_bNewDataAvailable;
	}

	virtual XnUInt64 GetTimestamp()
	{
		return m_pReader->GetTimestamp();
	}

	virtual XnUInt32 GetFrameID()
	{
		return m_pReader->GetFrameID();
	}

	//
	// Generator Start/Stop
	//

	virtual XnBool IsGenerating()
	{
		return m_pReader->IsRunning();
	}
	
	virtual XnStatus StartGenerating()
	{
		try {
			PreStartGenerating();
			m_pReader->AddListener(this);
			if (m_bActiveGeneratorControl) {
				CHECK_XN_STATUS(m_pReader->Start());
			}
			PostStartGenerating();
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}
	
	virtual void StopGenerating()
	{
		if (m_bActiveGeneratorControl) {
			m_pReader->Stop();
		}
		m_pReader->RemoveListener(this);
	}

protected:
	virtual void PreStartGenerating() {}
	virtual void PostStartGenerating() {}

};

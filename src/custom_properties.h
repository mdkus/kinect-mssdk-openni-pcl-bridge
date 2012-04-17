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

// IntProperty for depth node: Set to non-zero to get distinct overflow depth values
#define PROP_DEPTH_DISTINCT_OVERFLOW_DEPTH_VALUES "distinctOverflowDepthValues"
// IntProperty for depth node: Set to non-zero to enable near mode
#define PROP_DEPTH_NEAR_MODE "nearMode"

// IntProperty for all node types: Camera angle in degrees relative to the gravity. Use after generation started.
#define PROP_COMMON_CAMERA_ELEVATION_ANGLE "cameraElevationAngle"
// IntProperty for all node types: Get pointer value to INuiSensor that is ast to XnUInt64. Use after generation started. Read-only.
#define PROP_COMMON_NUI_SENSOR_POINTER "nuiSensorPointer"

// RealProperty for audio node: Beam angle. Use after generation started. Read-only.
#define PROP_AUDIO_BEAM_ANGLE "beamAngle"
// RealProperty for audio node: Audio source angle. Use after generation started. Read-only.
#define PROP_AUDIO_SOURCE_ANGLE "sourceAngle"
// RealProperty for audio node: Confidence of audio source angle. Use after generation started. Read-only.
#define PROP_AUDIO_SOURCE_ANGLE_CONFIDENCE "sourceAngleConfidence"

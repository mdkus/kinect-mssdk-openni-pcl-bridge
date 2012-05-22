kinect-mssdk-openni-bridge v1.5.0.0-for-1.5.2.23
Author: Tomoto S. Washio
Release Date: 5/22/2012
Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/

1. What is this?

Finally Microsoft released Kinect for Windows and Kinect SDK, but they
are not compatible with OpenNI. This kinect-mssdk-openni-bridge is an
experimental module that connects Kinect SDK to OpenNI and allows the
applications to use Kinect for Windows over OpenNI, so that you can
leverage the existing applications and know-hows on OpenNI with Kinect
for Windows.

This module plugs-in OpenNI framework with Depth, Image, User, and
Audio node that works with Kinect for Windows + Kinect SDK. The cool
part of this mechanism is it can work with other existing node
implementations from OpenNI + NITE. For example, the Depth node from
this module works with NITE's skeleton tracking and gesture recognition
algorithms. Another cool part is these nodes provide the access to
the features specific to Kinect for Windows through properties, such
as near mode, audio source detection, acoustic echo cancelling, etc.

2. Installation and Uninstallation

2.1 Prerequisites

This module is compatible with OpenNI 1.5.2.23 32bit and Kinect SDK V1.0.
You need to install them before installing this module.
* NOTE USE OpenNI 32BIT EVEN THOUGH YOUR OS MAY BE 64BIT.
* Most probably you can use newer version of OpenNI, but I do not know
  until when the compatibility stays.
* Kinect SDK Beta is not supported because the license is incompatible.
  Use the previous version if you really want to use it with Beta SDK.


2.2 Download

If you already have a copy of the binary package or the source
code repository, skip this section and go to Installation.

If not, download the binary package from the URL below, and extract
it somewhere in your computer. Source code is also available.
http://code.google.com/p/kinect-mssdk-openni-bridge/

2.3 Installation

After installing OpenNI and KinectSDK, run install.bat.
If your Windows turns on User Account Control (UAC),
run it "as administrator" by right-clicking the batch file.

If you need the debug version, use install-debug.bat instead.

To check if the installation is successfully done, run niReg
(an OpenNI utility) with -l option. If you find the name of this
module displayed, it is OK. Besides, you can run simple OpenNI examples
such as NiSimpleViewer and NiUserTracker to validate the installation.

2.4. Uninstallation

Run uninstall.bat. If your Windows turns on UAC, run it
"as administrator" as described above.

3. Usage

3.1 Basics

This module is automatically activated when you use Depth, Image,
and Audio node with Kinect SDK installed. You can use them with
other algorithms such as NITE's skeleton tracking and gesture
recognition.

If you want to use Kinect SDK's skeleton tracking instead of
NITE's UserGenerator, specify <Query> to the User node in the
configuration XML file as below. (Note the node names has been
changed from the previous version.)

<OpenNI>
    ...
    <ProductionNodes>
        ...
        <Node type="User" name="User1">
            <Query>
                <Name>KinectSDKUserSkeletonGenerator</Name>
            </Query>
        </Node>
        ...
    </ProductionNodes>
</OpenNI>

3.2 More tips

* Image node supports 640x480 (default) and 1280x1024 resolution.
  This example configures the Image node in the high resolution mode.

        <Node type="Image" name="Image1">
            <Configuration>
                <MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
            </Configuration>
        </Node>

* Depth node supports 640x480 (default) and 320x240 resolution.
  The resolution can be specified in the same way as Image node.
  When KinectSDKUserSkeletonGenerator node is used, the internal
  depth stream falls back to 320x240 transparently and emulates 640x480.
  This is due to Kinect SDK's limitation.
  
  Near mode is supported. Set nearMode int property to non-zero
  to turn it on.
  [Kinect SDK 1.5] You can get the skeleton data if you use
  KinectSDKUserSkeletonGenerator in near mode.

        <Node type="Depth" name="Depth1">
            <Configuration>
                <Property type="int" name="nearMode" value="1"/>
            </Configuration>
        </Node>

  Distinct overflow values (returns different values for too near, too far,
  and unknown distances) is also supported. Set distinctOverflowDepthValues
  int property to non-zero to turn it on.

        <Node type="Depth" name="Depth1">
            <Configuration>
                <Property type="int" name="distinctOverflowDepthValues" value="1"/>
            </Configuration>
        </Node>

* [Kinect SDK 1.5] By specifying XN_SKEL_PROFILE_HEAD_HANDS or
  XN_SKEL_PROFILE_UPPER to SetSkeletonProfile when you use
  KinectSDKUserSkeletonGenerator, the skeleton tracking works in Seated Mode.
  e.g. userGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_HEAD_HANDS)

* If your application assumes Psi pose detection, specify
  "KinectSDKUserSkeletonGeneratorWithPsiPoseEmulation" instead of
  "KinectSDKUserSkeletonGenerator". You application will get
  fake pose detection events.

* You can programatically get and set the camera elevation angle by
  cameraElevationAngle property of the Image, Depth, or User node
  from this module. Use this only after StartGenerating.

  // angle = camera elevation in degree relative to the horizon
  imageGenerator.SetIntProperty("cameraElevationAngle", angle);

  Also you can get the INuiSensor pointer associated with the node
  by nuiSensorPointer property so that you can directly use the
  Kinect SDK API functions. Use this only after StartGenerating.

  XnUInt64 i;
  imageGenerator.GetIntProperty("nuiSensorPointer", i);
  INuiSensor* pSensor = (INuiSensor*) i;
  pSensor->NuiXxx(...);

* There is a small fun stuff -- User node with auto elevation adjustment.
  Specify "KinectSDKUserSkeletonGeneratorWithAutoElevation" instead of
  "KinectSDKUserSkeletonGenerator", then the camera elevation will be
  auto-adjusted so that the whole player body fits within the sight.

* Audio node supports the configuration properties below.

        <Node type="Audio" name="Audio1">
            <Configuration>
                <!-- Beam forming mode
                     0: Adaptive (Controlled by Kinect-specific DSP)
                     1: Automatic (Controlled by DMO)
                     2: Manual (Controlled by the application via the beamAngle property explained later) -->
                <Property type="int" name="beamAngleMode" value="0"/>
                
                <!-- Automatic gain control: 0(off), 1(on) -->
                <Property type="int" name="automaticGainControl" value="0"/>
                
                <!-- Echo cancellation (AEC) and suppression of residual signal (AES)
                     0: None
                     1: AEC
                     2: AEC and AES once
                     3: AEC and AES twice -->
                <Property type="int" name="echoCancellationMode" value="2"/>
                
                <!-- Center clipping (a technique to remove the residual signal after AEC) and
                     Noise filling (a technique to make the sound natural by using a small
                     amount of noise instead of silence after removing the residual signal)
                     0: None
                     1: Center clipping
                     2: Center clipping and Noise filling
                     -->
                <Property type="int" name="centerClippingMode" value="0"/>
                
                <!-- Noise suppression: 0(off), 1(on) -->
                <Property type="int" name="noiseSuppression" value="1"/>
                
                <!-- Audio buffer size in milliseconds. The audio data is
                     lost unlsee the application polls the audio data
                     within this interval. Set before StartGenerating. -->
                <Property type="int" name="bufferSizeInMs" value="1000"/>
            </Configuration>
        </Node>

  Also, you can get the beam angle and the audio source angle from your
  application programatically.
  
  double beamAngle, sourceAngle, sourceAngleConfidence;
  audioGenerator.GetRealProperty("beamAngle", beamAngle); // read-only or read-write when beamAngleMode is manual
  audioGenerator.GetRealProperty("sourceAngle", sourceAngle); // read-only
  audioGenerator.GetRealProperty("sourceAngleConfidence", sourceAngleConfidence); // read-only

4. License

See LICENSE.TXT.

Notice:
This module is intended to encourage the use of Kinect for Windows and
OpenNI, but not intended to give any losses or damages to the providers
of the related software and devices including Microsoft and PrimeSense.
The user of this module is responsible for complying the license terms of
the software and devices such as Kinect for Windows, Kinect SDK, OpenNI,
NITE, etc. when the user combines them with this module. The author of
this module is not liable for any consequences caused by failing to do so.

5. Miscellaneous

* David Sherba kindly helped me to port the module from Beta to V1.0
  by contributing his study and mod. Thank you very much.

* You may ask me why I wrote everything in the header files or why I
  used templates maybe too much. It was because I was just bored to do
  things normally and felt like doing something weird. :-)

6. Change History

V1.5.0.0-1.5.2.23
* Kinect SDK V1.5 support (near mode skeleton tracking, seated mode)

V1.0.0.6-1.5.2.23
* Now properties are correctly saved to .oni file.
* Supported enumeration and selection from multiple Kinect sensors,
  but with limitations: The last sensor corresponding to the device
  node most recently created before StartGenerating will be selected.
  No simultaneous use of multiple sensors.

V1.0.0.5-1.5.2.23
* Better error message
* Fixed bug: AudioGenerator::GetAudioBuffer should not return null.

V1.0.0.4-1.5.2.23
* Officially supported Audio node
* nearMode and distinctOverflowDepthValues are now configurable even at
  runtime

V1.0.0.3-1.5.2.23
* Fixed Issue #1: No data when mirror capability is set to false
* Added first experimental implementation of Audio node with
  beamAngle, sourceAngle, sourceAngleConfidence property

V1.0.0.2-1.5.2.23
* Multiple sensor support (not completely tested)
* Supported access to camera elevation angle and INuiSensor pointer

V1.0.0.1-1.5.2.23
* Supported Kinect SDK V1.0 and revised the license terms
* Supported near mode
* Supported distinct overflow values
* Discontiuned Kinect SDK Beta support
* Changed the custom node names from MSRKinect... to KinectSDK...
* Changed the version number scheme:
  v<Kinect SDK version>.<Function version>-for-<OpenNI version>
* Bug fixes

V1.3.3.6
* Recompiled on OpenNI 1.3.3.6
* Supported Kinect SDK Beta2

V0.0
* First release for Kinect SDK Beta

Enjoy!
Tomoto S. Washio

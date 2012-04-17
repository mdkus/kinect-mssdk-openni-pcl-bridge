kinect-mssdk-openni-bridge v1.0.0.3-for-1.5.2.23
Author: Tomoto S. Washio
Release Date: 4/17/2012
Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/

1. What is this?

Finally Microsoft released Kinect for Windows and Kinect SDK (available
for commercial use), but they are not compatible with OpenNI. This is
unfortunate because even if you already had applications and know-hows
on OpenNI, you would not be able to leverage them with Kinect for Windows.

This kinect-mssdk-openni-bridge is an experimental module, the goal of
which is to solve this tragedic situation by connecting Kinect SDK to
OpenNI and allowing the applications to use Kinect for Windows over
OpenNI.

This module plugs-in OpenNI framework with Depth, Image, and User node
that works with Kinect for Windows + Kinect SDK. Especially, this Depth
node works with NITE's skeleton tracking and gesture recognition algorithms.

This module is compatible with OpenNI 1.5.2.23 32bit and Kinect SDK V1.0.
* Note OpenNI must be 32bit even though your OS may be 64bit.
* Kinect SDK Beta is not supported because the license is incompatible.
  Use the previous version if you really want to use it with Beta SDK.

2. Installation and Uninstallation

2.1 Download

If you already have a copy of the binary package or the source
code repository, skip this section and go to Installation.

If not, download the binary package from the URL below, and extract
it somewhere in your computer. Source code is also available.
http://code.google.com/p/kinect-mssdk-openni-bridge/

2.2 Installation

After installing OpenNI and KinectSDK, run install.bat.
If your Windows turns on User Account Control (UAC),
run it "as administrator" by right-clicking the batch file.

If you need the debug version, use install-debug.bat instead.

To check if the installation is successfully done, run niReg
(an OpenNI utility) with -l option. If you find the name of this
module displayed, it is OK.

2.3. Uninstallation

Run uninstall.bat. If your Windows turns on UAC, run it
"as administrator" as described above.

3. Usage

3.1 Basics

This module is automatically activated when you use Depth and
Image node with Kinect SDK installed. You can use them with
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

3.2 Tips

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

* If your application assumes Psi pose detection, specify
  "KinectSDKUserSkeletonGeneratorWithPsiPoseEmulation" instead of
  "KinectSDKUserSkeletonGenerator". You application will get
  fake pose detection events.

* There is a small fun stuff -- User node with auto elevation adjustment.
  Specify "KinectSDKUserSkeletonGeneratorWithAutoElevation" instead of
  "KinectSDKUserSkeletonGenerator", then the camera elevation will be
  auto-adjusted so that the whole player body fits within the sight.

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

V1.0.0.3-1.5.2.23
ÅEFixed Issue #1: No data when mirro capability is set to false
ÅEAdded first experimental implementation of Audio node with
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

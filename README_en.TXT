kinect-mssdk-openni-bridge V1.0-1.5.2.23
Author: Tomoto S. Washio
Release Date: 4/7/2012
Project URL: https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

1. What is this?

Finally Microsoft released Kinect for Windows and Kinect SDK (available
for commercial use), but they are not compatible with OpenNI. This is
unfortunate because even if you already had applications and know-hows
on OpenNI, you would not be able to leverage them with Kinect for Windows.

This kinect-mssdk-openni-bridge is an experimental module, the goal of
which is to solve this tragedic situation by connecting Kinect SDK to
OpenNI and allowing the applications to use Kinect for Windows over
OpenNI.

If you install this module, Depth, Image, and User node can (a sort of)
work with Kinect SDK.

This module is compatible with OpenNI 1.5.2.23 32bit and Kinect SDK V1.0.
- Note OpenNI must be 32bit even though your OS may be 64bit.
- Kinect SDK Beta is not supported because the license is incompatible.
  Use the previous version if you really want to use it with Beta SDK.

2. Installation and Uninstallation

2.1 Download

If you already have a copy of the binary package or the source
code repository, skip this section and go to Installation.

If not, download the binary package from the URL below, and
extract it somewhere in your computer.
https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes/release/

If you are interested in the source code, you can access to the
entire repository by git at:
https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

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

This module will be automatically activated for Depth and Image
nodes. If you want to use Kinect SDK's skeleton tracking instead of
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

4. License

See LICENSE.TXT.

Notice:
This module is intended to encourage the use of Kinect for Windows, but
not intended to give any losses or damages to the providers of the
related software and devices including Microsoft and PrimeSense.
The user of this module is responsible for complying the license terms of
the software and devices such as Kinect for Windows, Kinect SDK, OpenNI,
NITE, etc. when the user combines them with this module. The author of
this module is not liable for any consequences caused by failing to do so.

5. Acknowledgments

David Sherba kindly helped me to port the module from Beta to V1.0
by contributing his study and mod. Thank you very much.

6. Change History

V1.0-1.5.2.23 (4/2/2012)
- Supported Kinect SDK V1.0 and revised the license terms
- Supported near mode
- Supported distinct overflow values
- Discontiuned Kinect SDK Beta support
- Changed the custom node names from MSRKinect... to KinectSDK...
- Changed the version number scheme: <Kinect SDK version>-<OpenNI version>
- Bug fixes

V1.3.3.6 (11/5/2011)
- Recompiled on OpenNI 1.3.3.6
- Supported Kinect SDK Beta2

V0.0
- First release for Kinect SDK Beta

Enjoy!
Tomoto

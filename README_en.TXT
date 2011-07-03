kinect-mssdk-openni-bridge V0.0
Author: Tomoto S. Washio
Release Date: 7/2/2011
Project URL: https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

1. What is this?

Finally Microsoft's official Kinect SDK Beta is out!
This is good news for Kinect Hackers. However, unfortunately,
existing Kinect hacks built on OpenNI stop working if you
install this official SDK because of the driver conflict.

This kinect-mssdk-openni-bridge is an experimental module,
the goal of which is to solve this tragedic situation by
allowing the applications to use the offical SDK through OpenNI.

If you install this module, Depth, Image, and User node can
(a sort of) work with the official SDK installed.

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

After installing OpenNI, run install.bat. If your Windows
turns on User Account Control (UAC), run it "as administrator"
by right-clicking the batch file.

If you need the debug version, use install-debug.bat instead.

To check if the installation is successfully done, run niReg
(an OpenNI utility) with -l option. If you find the name of this
module displayed, it is OK.

2.3. Uninstallation

Run uninstall.bat. If your Windows turns on UAC, run it
"as administrator" as described above.

3. Usage

This module will be automatically activated for Depth and Image
nodes. For User node, you need to specify <Query> as below.

<OpenNI>
    ...
    <ProductionNodes>
        ...
        <Node type="User" name="User1">
            <Query>
                <Name>MSRKinectUserSkeletonGenerator</Name>
            </Query>
        </Node>
        ...
    </ProductionNodes>
</OpenNI>

Image node supports 640x480 (default) and 1280x1024 resolution.
This example configures the Image node in the high resolution mode.

		<Node type="Image" name="Image1">
			<Configuration>
				<MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
			</Configuration>
		</Node>

Depth node only supports 640x480 resolution. When User node is
used or AlternativeViewPointCap is used, it falls back to
320x240 internally to workaround Kinect SDK's limitation.

4. License

See LICENSE.TXT.

Enjoy!
Tomoto

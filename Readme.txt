kinect-mssdk-openni-pcl-bridge v1.0.0
Author: Michael Dingerkus <mdkus@web.de>
Release Date: 3/5/2013
Project URLs: https://github.com/mdkus/kinect-mssdk-openni-pcl-bridge
              http://code.google.com/r/mdkus-kinect-mssdk-openni-pcl-bridge/

License
-------------------------

Copyright (c) 2013, Michael Dingerkus
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the copyright holder(s) nor the names of their
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Additional Notice:
This software is intended to encourage the use of Kinect for Windows and
OpenNI, but never intended to give any losses or damages to Microsoft,
PrimeSense, or any other providers of the related software and devices.
When this software is used, the license terms of Kinect for Windows,
Kinect SDK, OpenNI, PCL, and any other related software and devices that are
combined with this software must also be complied, and the copyright holders
shall not be liable for any consequences caused by failing to do so.

The Windows Kinect SDK is a closed-source binary and thus can't be incorporated in
the BSD-licensed PCL distribution. 
So the 'Use-kinect-mssdk-openni-pcl-bridge-module-in-PCL.patch' can't be applied 
to the PCL project's remote repository. Nevertheless you can apply the patch to your 
own PCL clone for your own purposes under the conditions mentioned above.


1. Purpose

My intention was to use Microsofts Kinect for Windows camera, driven by Microsofts "Kinect SDK", with PCL ( "Point Cloud Library" 
Project URL: https://github.com/PointCloudLibrary/pcl ). 

Tomoto S. Washio's bridge module ( Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/ ) should enable to 
use this SDK with OpenNI and therefore the camera should work with PCL, but it didn't at first. 

I found out that the original module "kinect-mssdk-openni-bridge v1.6.0.0-for-1.5.2.23" by Tomoto S. Washio leads to some crashes when
I tested it with PCL's "pcl_kinfu_largeScale" application and other tools and demos of PCL. 

So I did some debugging and made some little changes to Tomoto's code and then it obviously worked. I named this version "kinect-mssdk-openni-pcl-bridge".
This forked PCL version of Tomoto's original project can be cloned from the remote repositories on the URLs mentioned at the top of this file.


2. Requirements

First you need a Microsoft Kinect for Windows camera. To use it you need to install the Kinect SDK which requires Windows 7 minimum. 
For additional information see http://msdn.microsoft.com/en-us/library/hh855359.aspx. 
Please read the original README of Tomoto S. Washio first:
Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/ 


3. Steps for using the 'kinect-mssdk-openni-pcl-bridge' module

  - Install Microsofts Kinect SDK for the Kinect for Windows camera.

  - Download the 'installV10.zip' file from one of the projects remote repositories mentioned at the top of this file.

  - Create a subdirectory named '3rdparty' in your PCL_BINARY_DIR and unzip (Freeware '7-Zip') the 'kinect-mssdk-openni-pcl-bridge.7z'
    into this new subdirectory. There will be a directory named 'kinect-mssdk-openni-pcl-bridge' which contains the necessary files and 
	directories.

  - Copy or Move the 'Use-kinect-mssdk-openni-pcl-bridge-module-in-PCL.patch' into your PCL_SOURCE_DIR and apply it to your PCL clone          
  
  - Re-run CMake to generate new Build-Files which can be compiled 

  - Last but not least: install the "kinect-mssdk-openni-pcl-bridge-V10Release.dll" or "kinect-mssdk-openni-pcl-bridge-V10Debug.dll"
    to OpenNI! Please read the original README of Tomoto S. Washio to do this ("Readme_en.txt"). Use the *.bat-executables in the 
    'kinect-mssdk-openni-pcl-bridge' directory.


4. Additional information

Please note that this modified project is a fork of the original project of Tomoto S. Washio. 

The modified bridge module was successfully tested with "pcl_kinfu_largeScale". Other apps might work as well if they use raw bayer format too.
Up to this point the module only provides image data in raw bayer format. To get RGB values it must be re-compiled because it is hard-coded yet.

The Kinect SDK only provides Image-stream or! IR-stream at the same time. This might be a disadvantage. But there are a
advantages as well: p.e. the "Near mode", a minimum depth sensoring of 0,4 m.

If you want to re-compile the 'kinect-mssdk-openni-pcl-bridge' project you have to

  - get a clone of the remote repository (URLs at the top of this file) in your new directory '3rdparty'
   (if you don't have git installed yet please install it first). The name of the clone must be 'kinect-mssdk-openni-pcl-bridge'.
    If the name of the clone differs rename it.

  - re-compile the project (I used MSVC10), the modules will be generated in the 'bin'-directory

  - if you generate a debug version of the bridge module, it's better to uninstall the release version in OpenNI first 
    before you install the debug version to OpenNI because OpenNI calls back functions of all! installed modules, 
	which decreases performance

  - in order to debug your bridge module dll-project with PCL's "pcl_kinfu_largeScale" app type the 
    following command for execution (MSVC):
	
	  (..\)..\..\bin\pcl_kinfu_largeScale_debug.exe (alternatively you can use an absolute path)

	add the following command options for "registration" and "extracting textures" when running the app:

	  -r -et 


5. Miscellaneous

* Thanks to Tomoto S. Washio for his original bridge module. He did a really great job.


6. Change History

V1.0.0
* Tested with PCL's "kinfu_largeScale"-app and other demos and tools  
* Image data in raw bayer format
* Modifications for the use with PCL

The PCL version of the bridge module is based on Tomoto S. Washio's latest Release (10/9/2012):

V1.6.0.0-1.5.2.23
* Tested on Kinect SDK V1.6
* IR node support
* accelerometer and forceInfraredEmitterOff properties support

...

7. Maintaining

If you have any questions, suggestions and proposals, don't hesitate to contact Michael Dingerkus <mdkus@web.de>.


kinect-mssdk-openni-bridge V0.0
Author: Tomoto S. Washio
Release Date: 2011/6/22
Project URL: https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

1. これは何？

先日Microsoftから公式Kinect SDKのBeta版がリリースされました。
これはKinectハッカーにはうれしいニュースですが、残念ながら、
公式SDKをインストールすると、ドライバの衝突のため、OpenNI上で
作られた従来のKinectハックが動かなくなってしまいます。

この残念な状況を打開するため、公式SDKのドライバをOpenNI上から
使えるようにする実験モジュールがこのkinect-mssdk-openni-bridgeです。

このモジュールをインストールすると、DepthノードとImageノードが
公式SDK上で動作するようになります。SceneノードやUserノードは
残念ながらまだ動きません。

2. インストール

OpenNIをインストールした上で、install.batを「管理者として実行」
してください。

デバッグ版をインストールするには代わりにinstall-debug.batを
実行してください。

正しくインストールされたかどうかはOpenNIのユーティリティniRegを
-lオプションをつけて呼び出すことで確認できます。本モジュールの
名前が表示されていれば成功です。

3. アンインストール

uninstall.batを「管理者として実行」してください。
デバッグ版をインストールしている場合には代わりに
uninstall-debug.batを実行してください。

4. ライセンス

LICENSE.TXTを見てください。

Enjoy!
Tomoto

kinect-mssdk-openni-bridge v1.0.0.1-for-1.5.2.23
Author: Tomoto S. Washio
Release Date: 2012/4/12
Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/

1. これは何？

先日Microsoftから商用利用可能なKinect for WindowsとKinect SDKが
リリースされましたが、これらとOpenNIとの間には互換性がありません。
すなわち、OpenNI上のアプリケーション資産やノウハウが既にあったとしても、
Kinect for Windowsと組み合わせて活用することは不可能だったということです。

この残念な状況を打開するため、Kinect SDKをOpenNIに接続することで、
Kinect for WindowsをOpenNI上から使えるようにする実験モジュールが
このkinect-mssdk-openni-bridgeです。

このモジュールをインストールすると、Kienct for Windows + Kinect SDK上で
動作するDepthノード、Imageノード、UserノードがOpenNIにプラグインされます。
このDepthノードの上でNITEのスケルトントラッキングやジェスチャー検出を
使うこともできます。

本モジュールはOpenNI 1.5.2.23 32bit版およびKinect SDK V1.0に
対応しています
・OpenNIは、OSが64bitであっても必ず32bit版を使ってください。
・Kinect SDK Beta版はライセンスが異なるため対応していません。
  Beta版で使用したい場合には以前のバージョンを使用してください。

2. インストールとアンインストール

2.1 ダウンロード

既にバイナリパッケージまたはソースコードリポジトリのコピーを
入手しているならば、本節を飛ばしてインストールに進んでください。

そうでなければ、バイナリパッケージを次のURLからダウンロードし、
どこかに展開してください。ソースコードもあります。
http://code.google.com/p/kinect-mssdk-openni-bridge/

2.2 インストール

OpenNIとKinectSDKをインストールした上で、install.batを実行してください。
Windowsのユーザーアカウント制御(UAC)が有効になっている場合、
バッチファイルを右クリックして「管理者として実行」してください。

デバッグ版をインストールするには代わりにinstall-debug.batを
実行してください。

正しくインストールされたかどうかはOpenNIのユーティリティniRegを
-lオプションをつけて呼び出すことで確認できます。本モジュールの
名前が表示されていれば成功です。

2.3. アンインストール

uninstall.batを実行してください。UACが有効になっている場合、
上と同様に「管理者として実行」してください。

3. 使い方

3.1 基本

Kinect SDKが入った環境でDepthノードとImageノードを使うと、
自動的に本モジュールが呼ばれるようになります。これらのノードは
NITEのスケルトントラッキングやジェスチャー認識など他のアルゴリズムと
組み合わせて使用することができます。

NITEの標準のスケルトントラッキングの代わりにKinect SDKのものを
使用するには設定XMLファイルのUserノードに箇所に次のように<Query>を
指定してください。(ノード名が旧版から変わっているので注意してください。)

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

3.2 応用

・Imageノードは640x480(デフォルト)と1280x1024の解像度をサポートします。
  次のように指定するとImageノードを高解像モードで使用できます。

		<Node type="Image" name="Image1">
			<Configuration>
				<MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
			</Configuration>
		</Node>

・Depthノードは640x480(デフォルト)と320x240の解像度をサポートします。
  解像度の指定の仕方はImageノードと同じです。
  KinectSDKUserSkeletonGeneratorノードを使うと、
  内部的に320x240の解像度で動作し640x480をエミュレートします。
  (Kinect SDKの制限のため)

  近接モードがサポートされています。近接モードを有効にするには次のように
  nearMode整数プロパティにゼロ以外の値を設定します。

		<Node type="Depth" name="Depth1">
			<Configuration>
				<Property type="int" name="nearMode" value="1"/>
			</Configuration>
		</Node>

  またオーバーフロー値の区別(近すぎる、遠すぎる、不明な距離に対して
  異なる値を返す)もサポートされています。これを有効にするには次のように
  distinctOverflowDepthValues整数プロパティにゼロ以外の値を設定します。

		<Node type="Depth" name="Depth1">
			<Configuration>
				<Property type="int" name="distinctOverflowDepthValues" value="1"/>
			</Configuration>
		</Node>

・あなたのアプリケーションがPsiポーズ検出を前提としている場合、
  "KinectSDKUserSkeletonGenerator" の代わりに
  "KinectSDKUserSkeletonGeneratorWithPsiPoseEmulation" を指定してください。
  偽者ですがポーズ検出イベントを受け取れるようになります。

・ちょっと遊び心で「自動角度調整機能付きUserノード」を作ってあります。
  "KinectSDKUserSkeletonGenerator" の代わりに
  "KinectSDKUserSkeletonGeneratorWithAutoElevation" を指定すると、
  プレイヤー全体が視界に収まるよう自動的にカメラの角度が調整されます。

4. ライセンス

LICENSE.TXTを見てください。

注意:
本モジュールはKinect for WindowsやOpenNIの利用促進を意図したものであり、
MicrosoftやPrimeSenseなど関連ソフトウェアやデバイスの提供者に
いかなる不利益をも与えることを意図したものではありません。
本モジュールの利用者は、Kinect for Windows、Kinect SDK、OpenNI、NITE等、
利用者が本モジュールと組み合わせて使用する関連ソフトウェアやデバイスの
ライセンスを遵守せねばなりません。利用者がこれに従わなかったために生じた
いかなる結果に対しても、本モジュールの作者は責任を負いません。

5. その他

・Kinect SDK Beta版からV1.0版への移植にあたり、David Scherbaさんに
  調査結果とコードとを提供して戴きました。この場で御礼申し上げます。

・なぜ全コードをヘッダに書いているのか、テンプレート使いすぎじゃないのか、
  と思った人へ。普通にコードを書くのに飽きたので、ちょっと変わったことを
  やってみたかっただけです。:-)

6. 更新履歴

V1.0-1.5.2.23
・Kinect SDK V1.0をサポートしライセンスを変更
・近接モードのサポート
・オーバーフロー値の区別のサポート
・Kinect SDK Betaのサポートを打ち切り
・カスタムノード名を MSRKinect... から KinectSDK... に変更
・バージョンスキームを変更:
  v<Kinect SDK version>.<Function version>-for-<OpenNI version>
・バグ修正

V1.3.3.6
・OpenNI 1.3.3.6上で再コンパイル
・Kinect SDK Beta2をサポート

V0.0
・Kinect SDK Beta向け初版

Enjoy!
Tomoto S. Washio

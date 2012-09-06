kinect-mssdk-openni-bridge v1.5.0.3-for-1.5.2.23
Author: Tomoto S. Washio
Release Date: 2012/9/6
Project URL: http://code.google.com/p/kinect-mssdk-openni-bridge/

1. これは何？

先日Microsoftから商用利用可能なKinect for WindowsとKinect SDKが
リリースされましたが、これらとOpenNIとの間には互換性がありません。
このkinect-mssdk-openni-bridgeは、Kinect SDKをOpenNIに接続することで
Kinect for WindowsをOpenNI上から使えるようにする実験モジュールです。
これにより、既存のOpenNI上のアプリケーション資産やノウハウを
Kinect for Windowsと組み合わせて活用することができるようになります。

このモジュールをインストールすると、Kienct for Windows + Kinect SDK上で
動作するDepthノード、Imageノード、Userノード、AudioノードがOpenNIに
プラグインされます。特筆すべきは、これらのノードはOpenNI + NITEによって
提供されている既存のノードと組み合わせて使用できる点です。例えば、
このモジュールのDepthノードの上でNITEのスケルトントラッキングや
ジェスチャーを使うことが可能です。さらには、これらのノードを使うと、
プロパティを通してKinect for Windowsの各種固有機能(近接モード、
音源方向検出、エコーキャンセリング等)を使うことができます。

2. インストールとアンインストール

2.1 前提条件

本モジュールはOpenNI 1.5.2.23 32bit版とKinect SDK V1.0またはV1.5の
組み合わせに対応しています。事前にこれらをインストールしておいてください。
・OpenNIは、OSが64bitであっても必ず32bit版を使ってください。
・コンパイルは1.5.4.0上で行っていますが1.5.2.23でも動くはずです。
・さらに新しい版のOpenNIも使用可能なはずですが、いつ互換性が
  崩れるかはわかりません。
・Kinect SDK Beta版はライセンスが異なるため対応していません。
  Beta版で使用したい場合には以前のバージョンを使用してください。

2.2 ダウンロード

既にバイナリパッケージまたはソースコードリポジトリのコピーを
入手しているならば、本節を飛ばしてインストールに進んでください。

そうでなければ、バイナリパッケージを次のURLからダウンロードし、
どこかに展開してください。ソースコードもあります。
http://code.google.com/p/kinect-mssdk-openni-bridge/

2.3 インストール

OpenNIとKinectSDKをインストールした上で、install.batを実行してください。
Windowsのユーザーアカウント制御(UAC)が有効になっている場合、
バッチファイルを右クリックして「管理者として実行」してください。

デバッグ版をインストールするには代わりにinstall-debug.batを
実行してください。

正しくインストールされたかどうかはOpenNIのユーティリティniRegを
-lオプションをつけて呼び出すことで確認できます。本モジュールの
名前が表示されていれば成功です。また、OpenNIの簡単なサンプルを
実行することにより確認することもできます。

2.4. アンインストール

uninstall.batを実行してください。UACが有効になっている場合、
上と同様に「管理者として実行」してください。

3. 使い方

3.1 基本

Kinect SDKが入った環境でDepth、Image、あるいはAudioノードを使うと、
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
  [Kinect SDK 1.5の場合] 近接モードでKinectSDKUserSkeletonGeneratorを
  使うとスケルトンが取得できます。

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

  [Kinect SDK 1.5の場合] SetSkeletonProfileにXN_SKEL_PROFILE_HEAD_HANDS
  またはXN_SKEL_PROFILE_UPPERを指定してKinectSDKUserSkeletonGeneratorを使うと
  スケルトントラッキングがSeated modeになります。
  例: userGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_HEAD_HANDS)

・あなたのアプリケーションがPsiポーズ検出を前提としている場合、
  "KinectSDKUserSkeletonGenerator" の代わりに
  "KinectSDKUserSkeletonGeneratorWithPsiPoseEmulation" を指定してください。
  偽者ですがポーズ検出イベントを受け取れるようになります。

・本モジュールのImage、Depth、UserノードはcameraElevationAngleプロパティを
  備えており、これを用いてプログラムからカメラの角度を取得・設定することが
  できます。StartGeneratingを呼び出した後に使用することができます。

  // angle = 水平線に対するカメラの角度(度)
  imageGenerator.SetIntProperty("cameraElevationAngle", angle);

  また、nuiSensorPointerプロパティでINuiSensorへのポインタを取得し、
  Kinect SDKのAPI関数を直接使用することができます。StartGeneratingを
  呼び出した後に使うことができます。

  XnUInt64 i;
  imageGenerator.GetIntProperty("nuiSensorPointer", i);
  INuiSensor* pSensor = (INuiSensor*) i;
  pSensor->NuiXxx(...);

・ちょっと遊び心で「自動角度調整機能付きUserノード」を作ってあります。
  "KinectSDKUserSkeletonGenerator" の代わりに
  "KinectSDKUserSkeletonGeneratorWithAutoElevation" を指定すると、
  プレイヤー全体が視界に収まるよう自動的にカメラの角度が調整されます。

・本モジュールのAudioノードには次の設定プロパティが用意されています。

        <Node type="Audio" name="Audio1">
            <Configuration>
                <!-- ビーム形成(指向性決定)モード
                     0: Adaptive (Kinect固有のDSPにより制御)
                     1: Automatic (DMOにより制御)
                     2: Manual (後述のbeamAngleプロパティに値を設定することでアプリにより制御) -->
                <Property type="int" name="beamAngleMode" value="0"/>
                
                <!-- 自動感度調整: 0(off), 1(on) -->
                <Property type="int" name="automaticGainControl" value="0"/>
                
                <!-- エコーキャンセル(AEC)と残留エコーの抑制(AES) -->
                     0: なし
                     1: AEC
                     2: AECとAES 1回
                     3: AECとAES 2回 -->
                <Property type="int" name="echoCancellationMode" value="2"/>
                
                <!-- Center clipping (AEC後の残留エコーを除去する技術)と
                     Noise filling (残留エコー除去部を無音にせず小さなノイズを加えて自然に聞かせる技術)
                     0: なし
                     1: Center clipping
                     2: Center clippingとNoise filling
                     -->
                <Property type="int" name="centerClippingMode" value="0"/>
                
                <!-- ノイズ抑制: 0(off), 1(on) -->
                <Property type="int" name="noiseSuppression" value="1"/>
                
                <!-- 音声データバッファのサイズ(ミリ秒)
                     アプリはこの間隔以内でデータを取得しないとデータが失われる可能性がある。
                     StartGenerating前に設定すること。 -->
                <Property type="int" name="bufferSizeInMs" value="1000"/>
            </Configuration>
        </Node>

  また、次のようにアプリケーションからビーム方向と音源方向をプログラム的に
  取得することができます。
  
  double beamAngle, sourceAngle, sourceAngleConfidence;
  audioGenerator.GetRealProperty("beamAngle", beamAngle); // 読み取り専用、または読み書き可能(beamAngleModeがManualの場合)
  audioGenerator.GetRealProperty("sourceAngle", sourceAngle); // 読み取り専用
  audioGenerator.GetRealProperty("sourceAngleConfidence", sourceAngleConfidence); // 読み取り専用

* bin/kinect-mssdk-openni-bridge-V10.ini をいじることで全体の設定を
  変更できます。詳細はファイル内の説明を参照してください。

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

V1.5.0.3-1.5.2.23
* AlternateViwePointの性能向上(Thanks to Tony)
* ErrorStateCapをサポート、センサーの切断を検出可能に
* Issue #9 AlternateViewPoint時に深度情報に穴が生じる問題の修正
* creation infoのフォーマットをPCLのOpenNIGrabberが解析できるよう改良
　(Issue #10で指摘されたもの)
* 全体設定ファイルのサポート(.ini)

V1.5.0.2-1.5.2.23
* インストーラのKinect SDKバージョンチェックを削除。
* ContextをReleaseした後の最初期化をサポート。

V1.5.0.1-1.5.2.23
* プロパティShadowValueとNoSampleValueを追加。あるOpenCVアプリで必要。

V1.5.0.0-1.5.2.23
* Kinect SDK V1.5サポート(近接モードでのスケルトントラッキング、
　seatedモード)

V1.0.0.6-1.5.2.23
・oniファイルへのプロパティ保存を実装
・複数Kinectの列挙と選択をなんとなくサポート
  (選択は制限あり。StartGenerating前に最後に作られたDeviceノードに対応する
   Kinectが選択される。複数同時選択は不可能。)

V1.0.0.5-1.5.2.23
・エラーメッセージ改善
・AudioGenerator::GetAudioBufferがNULLを返すバグを修正

V1.0.0.4-1.5.2.23
・Audioノードを正式サポート
・nearModeとdistinctOverflowDepthValuesの実行中の変更をサポート

V1.0.0.3-1.5.2.23
・Issue #1 (mirrorをfalseにしたときにデータが取得できなくなるバグ)を修正
・AudioノードとbeamAngle, sourceAngle, sourceAngleConfidenceプロパティの
  試験実装を追加

V1.0.0.2-1.5.2.23
・複数センサーのサポート (未テスト)
・カメラ角度とINuiSensorポインタへのアクセスをサポート

V1.0.0.1-1.5.2.23
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

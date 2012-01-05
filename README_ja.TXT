kinect-mssdk-openni-bridge V1.3.3.6
Author: Tomoto S. Washio
Release Date: 2011/11/5
Project URL: https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

1. これは何？

先日Microsoftから公式Kinect SDKのBeta版がリリースされました。
これはKinectハッカーにはうれしいニュースですが、残念ながら、
公式SDKをインストールすると、ドライバの衝突のため、OpenNI上で
作られた従来のKinectハックが動かなくなってしまいます。

この残念な状況を打開するため、公式SDKのドライバをOpenNI上から
使えるようにする実験モジュールがこのkinect-mssdk-openni-bridgeです。

このモジュールをインストールすると、Depthノード、Imageノード、
Userノードが公式SDK上で(なんとなく)動作するようになります。

OpenNI 1.3.3.6 32bit版に対応しています。
(OSが64bitであっても32bit版を使ってください。)
KinectSDKはBeta1対応版とBeta2対応版の両方が含まれており、
インストール時に自動的に選択されます。

2. インストールとアンインストール

2.1 ダウンロード

既にバイナリパッケージまたはソースコードリポジトリのコピーを
入手しているならば、本節を飛ばしてインストールに進んでください。

そうでなければ、バイナリパッケージを次のURLからダウンロードし、
どこかに展開してください。
https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes/release/

もしソースコードに興味があるならば、次のURLにあるソースコード
リポジトリにgitでアクセスすることができます。
https://www.assembla.com/code/kinect-mssdk-openni-bridge/git/nodes

2.2 インストール

OpenNIとKinectSDKをインストールした上で、install.batを実行してください。
Windowsのユーザーアカウント制御(UAC)が有効になっている場合、
バッチファイルを右クリックして「管理者として実行」してください。

デバッグ版をインストールするには代わりにinstall-debug.batを
実行してください。

正しくインストールされたかどうかはOpenNIのユーティリティniRegを
-lオプションをつけて呼び出すことで確認できます。本モジュールの
名前が表示されていれば成功です。

なお、この手順をわかりやすく説明した記事をkaorun55さんに
書いていただきました。合わせて参照してください。
http://d.hatena.ne.jp/kaorun55/20110627/1309193339

2.3. アンインストール

uninstall.batを実行してください。UACが有効になっている場合、
上と同様に「管理者として実行」してください。

3. 使い方

3.1 基本

DepthノードとImageノードは何もしなくても自動的に動作します。
Kinect SDKのスケルトントラッキングを使用するには設定XMLファイルの
Userノードに箇所に次のように<Query>を指定してください。
これでPsiポーズが不要になります。
これを指定しない場合にはNITEの標準のUserGeneratorが動作します。

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

3.2 応用

・Imageノードは640x480(デフォルト)と1280x1024の解像度をサポートします。
  次のように指定するとImageノードを高解像モードで使用できます。

		<Node type="Image" name="Image1">
			<Configuration>
				<MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
			</Configuration>
		</Node>

・Depthノードは640x480(デフォルト)と320x240の解像度をサポートします。
  MSRKinectUserSkeletonGeneratorノードを使う、
  あるいはAlternativeViewPointCapを使うと、
  内部的に320x240の解像度で動作し640x480をエミュレートします。
  (Kinect SDKの制限を 回避するため)

・あなたのアプリケーションがPsiポーズ検出を前提としている場合、
  "MSRKinectUserSkeletonGenerator" の代わりに
  "MSRKinectUserSkeletonGeneratorWithPsiPoseEmulation" を指定してください。
  偽者ですがポーズ検出イベントを受け取れるようになります。

・ちょっと遊び心で「自動角度調整機能付きUserノード」を作ってあります。
  "MSRKinectUserSkeletonGenerator" の代わりに
  "MSRKinectUserSkeletonGeneratorWithAutoElevation" を指定すると、
  プレイヤー全体が視界に収まるよう自動的にカメラの角度が調整されます。

4. ライセンス

LICENSE.TXTを見てください。

Enjoy!
Tomoto

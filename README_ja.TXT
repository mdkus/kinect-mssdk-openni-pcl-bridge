kinect-mssdk-openni-bridge V0.0
Author: Tomoto S. Washio
Release Date: 2011/7/2
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

OpenNIをインストールした上で、install.batを実行してください。
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

DepthノードとImageノードは何もしなくても自動的に動作します。
Userノードは設定XMLファイルに次のように<Query>を記述してください。

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

Imageノードは640x480(デフォルト)と1280x1024の解像度をサポートします。
次のように指定するとImageノードを高解像モードで使用できます。

		<Node type="Image" name="Image1">
			<Configuration>
				<MapOutputMode xRes="1280" yRes="1024" FPS="15"/>
			</Configuration>
		</Node>

Depthノードは640x480の解像度のみサポートしています。
Userノードを使う、あるいはAlternativeViewPointCapを使うと、
内部的に320x240の解像度で動作します。(Kinect SDKの制限を
回避するため)


4. ライセンス

LICENSE.TXTを見てください。

Enjoy!
Tomoto

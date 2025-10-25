= センサーを使ってモニタリングしよう

ここでは、いよいよ各種センサーを使ったデータの取得方法から、最終的に一覧で見やすくまとめるところまでを解説していきます。


ここで紹介するセンサーは全て用意する必要はありません。まずは自分が必要だと思うものから
・用意するもの

== 購入品一式

//table[sensor_list][購入品一式]{
品目	個数	購入先	目安の価格
------------------
Seeeduino Lotus@<fn>{seeeduino_lotus}	1つ	Seeed Studio	¥10,800
温度センサー@<fn>{temperature_sensor}	1つ	Seeed Studio	¥1,080
照度センサー@<fn>{light_sensor}	1つ	Seeed Studio	¥1,080
音量センサー@<fn>{volume_sensor}	1つ	Seeed Studio	¥1,080
水位センサー@<fn>{water_level_sensor}	1つ	Seeed Studio	¥1,080

//}

//TODO 正しいリンクと価格
//footnote[seeeduino_lotus][@<href>{https://www.seeedstudio.com/Seeeduino-Lotus-p-2517.html} Seeeduino Lotus]
//footnote[temperature_sensor][@<href>{https://www.seeedstudio.com/Grove-Temperature-Sensor-V1.0-p-1142.html} 温度センサー]
//footnote[light_sensor][@<href>{https://www.seeedstudio.com/Grove-Light-Sensor-p-1141.html} 照度センサー]
//footnote[volume_sensor][@<href>{https://www.seeedstudio.com/Grove-Sound-Sensor-p-1000207.html} 音量センサー]
//footnote[water_level_sensor][@<href>{https://www.seeedstudio.com/Grove-Water-Sensor-p-1000206.html} 水位センサー]

=== Seeeduino製品の選定の理由

Seeeduino Lotusはマイコンボードで有名なArduino Unoの互換機です。

Arduinoは非常に人気かつ安価なマイコンボードで、世界中で多くの人が使っています。

Arduinoに関する情報はネット上に多く公開されています。関連書籍も多いです。

Arduinoは基本的に、ブレッドボードを使って、センサーを接続して、プログラムを書いて、センサーのデータを取得するという流れで利用します。

そこで問題になるのが、配線です。

適切な配線をするためには、電子工作の知識が必要になってきます。
電圧・電流についての正しい理解と、ブレッドボードへの配線作業、そしてワイヤーをセンサーに接続するためのはんだ付けが必要になってきます。

そうなると難易度が一気に上がります。少しでも間違ってしまうと、後戻りも非常に大変で、そこで心が折れてしまう、学習が止まってしまうと言う事態は極力避けたかったという背景があります。

というかもう少しぶっちゃけて言うと、私にそんな知識もないし学習のモチベーションもどうしても沸かず、やりたくありませんでした。

カチャっと繋げて、はい完成みたいなプラモデルのような感覚で、IoTのセンサーを使いたかったんです。

そしてそんな私の怠惰な願望を叶えてくれたのが、Seeed社が販売している SeeeduinoとGroveコネクタで接続可能なセンサーだったんです。

Seeed社の公式ストアSeeed Studioで販売されているセンサーは、SeeeduinoとGroveコネクタで接続可能なセンサーが豊富に揃っています。

Seeeduino LotusはGroveコネクタを12個搭載していますので、今回私が紹介したセンサー以外にも、自分の好きなセンサーを接続することもできます。

ぜひ自分の好きなセンサーを接続して、楽しんでください。

今回ここには紹介していませんが、
ECセンサーを使っています。

しかし、公式サイトでは在庫がなく、メールで問い合わせたところ、今後の入荷も未定で代替製品もないとのことでした。
国内のECサイトでも取り扱いがないか在庫切れで、購入できない状況のため、紹介は省略しました。@<fn>{ec_sensor}

== センサーの接続方法

センサーの接続方法は非常に簡単です。

Seeeduino Lotusに12個搭載されているGroveコネクタと呼ばれるコネクタに、差し込むだけです。

ただし、Groveコネクタはセンサーの種類によって、差し込むコネクタの位置が異なります。

そのため、次の図を参考に配線を行ってください。

== センサーデータ取得のプログラム作成

それでは、センサーデータを取得するプログラムを作成していきます。

手順は次のとおりです。

 1. GitHubから一定感覚でセンサーデータを取得するプログラムのソースコードを取得する
 1. ソースコードをArduino IDEに貼り付ける
 1. プログラムをSeeeduino Lotusにアップロードする
 1. 出力結果を確認する
 1. Seeeduino Lotusをラズベリーパイに接続する
 1. ラズベリーパイで、Seeeduino Lotusから取得したセンサーデータをThingSpeakに送信するプログラムを実行する
 1. 結果をThingSpeakで確認する
 1. Grafanaのダッシュボードを更新する

=== Arduino IDEのインストール

まず、公式サイトのダウンロードページにアクセスして、使用するOSに応じたインストーラを選択します。

==== Windowsの場合

ダウンロードしたら、インストーラを実行して、インストールを行います。
インストールの際にいろいろ聞かれますが、特別な理由がなければ、デフォルトのままインストールを行ってください。インストール先はデフォルトのままで良いでしょう。
インストール完了後、Arduino IDEを起動して、初期画面（空のスケッチ画面）が表示されれば成功です。

==== Macの場合

CPU（Intel or Apple Silicon）の種類に注意してダウンロードしたら、インストーラを実行して、インストールを行います。

初回起動時には、セキュリティの警告が表示されますが、[開く]を選択して、続行します。
Arduino IDEを起動して、初期画面（空のスケッチ画面）が表示されれば成功です。

=== サードパーティ製ボードのインストール

Arduino

ちなみに、Visual Studio Code 拡張のPlatformIOというのもあります。
今回のようなシンプルなプログラムであれば、Arduino IDEでも十分です。
もし、自分の好きなエディタを使いたい場合は、PlatformIOをVisual Studio Codeにインストールしてください。@<fn>{platformio}

//footnote[platformio][@<href>{Visual Studio Code 拡張の方が、AIツールによる支援が受けやすいというメリットがあります。もし自分でプログラムを修正したい、という場合は、PlatformIOを使うと良いでしょう。}]

GroveコネクタはSeeeduino Lotusに12個搭載されていますので、センサーを接続するためには、Groveコネクタを使用します。

GroveコネクタはSeeeduino Lotusに12個搭載されていますので、センサーを接続するためには、Groveコネクタを使用します。

//footnote[ec_sensor][私はオーストラリアのECサイトで販売されているのを見つけ、購入しました。ただ、校正液を使った校正の必要があったり、正しいEC値の取得に難儀しています。]

★センサーが全てなくてもエラーにならないような工夫

★さらにやってみよう
	・閾値を設定した通知機能
	・他のセンサー
		・土壌水分量、EC値
		・
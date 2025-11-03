= すべてのデータをダッシュボードに反映させよう

@<chapref>{140_sensor}では、センサーの値を取得することができました。

ここまでの集大成として、Seeduinoから取得したデータもまとめてThingSpeakに送信し、Grafanaでダッシュボードを作成する手順を紹介します。

以下の順番で作業を行なっていきます。

 1. Seeeduino Lotusをラズベリーパイに接続する
 1. ラズベリーパイで、Seeeduino Lotusから取得したセンサーデータをThingSpeakに送信するプログラムを実行する
 1. 結果をThingSpeakで確認する
 1. Grafanaのダッシュボードを更新する


== Raspberry Piでシリアルデータを受信する
Seeduinoから取得したセンサーデータをRaspberry Piで受信するために、双方を接続する必要があります。

Raspberry PiのUSBポートとSeeeduino LotusのMicro USBポートを接続するケーブルを用意します。

TODO 画像 こんなの

Raspberry Pi側で、次のコマンドでデバイス名を確認します。

//cmd{
$ ls /dev/ttyACM*
//}

一般的には @<code>{/dev/ttyACM0} です。

シリアル受信できるかを確認します。

//cmd{
$ screen /dev/ttyACM0 115200
//}

シリアル受信ができていれば、結果がこのように表示されます。

TODO 画像

このデータをThingSpeakに送信しましょう。

== ThingSpeakに送信する

@<chapref>{120_dashboard} でThingSpeakのアカウントを作成し、チャンネルを作成しました。
そこに新たにフィールドを追加します。

// TODO 画像

ThingSpeak側の準備ができたら、データ送信のPythonスクリプトを準備します。

こちらのリポジトリにあるPythonスクリプトを使用します。

@<href>{https://github.com/Mutsumix/Seeeduino}

ラズベリーパイ上の任意の場所でGitを使ってクローンします。
ここではデスクトップ上に「Seeeduino」というフォルダを作成して、そこにクローンする想定で進めていきます。

//cmd{
git clone https://github.com/Mutsumix/Seeeduino.git
//}

設定ファイル（example.config.yml）をコピーして、config.ymlという名前で保存します。

//cmd{
cp thingspeak-uploader/example.config.yml thingspeak-uploader/config.yml
//}

config.ymlを開いて、ThingSpeakのAPIキーと、@<code>{ $ ls /dev/ttyACM* } で確認したデバイス名を設定します。

//emlist[config.yml]{
thingspeak_api_key: "YOUR_THINGSPEAK_API_KEY"
serial_port: "/dev/ttyUSB0"
baud_rate: 115200
send_interval_minutes: 0.25
//}

次に実行に必要なPythonのパッケージをインストールします。

//cmd{
pip install pyserial pyyaml requests
//}

最後に、実行します。

//cmd{
python uploader.py
//}

これでThingSpeakに設定した間隔でデータがアップロードされます。
デフォルトは0.25分（15秒）ので、好みの時間に設定すると良いでしょう。

TODO キャプチャ

== Grafanaでダッシュボードを作る

ThingSpeakでデータが受信できているのを確認できたら、最後に、Grafanaのダッシュボードにデータを追加します。

@<chap>{120_dashboard} でGrafanaの設定をしていれば、データソースとしてThingSpeakがすでに設定されているはずなので、データソースの設定は不要です。

温度・湿度・CO2を設定したときと同様に、各センサーのデータをグラフに設定していきます。

筆者はこのようにダッシュボードを作成しました。

//image[seeeduino-dashboad][Grafanaダッシュボードのサンプル][scale=0.75]

== まとめとさらなる活用について

この章では、SeeduinoからRaspberry Piにセンサーデータを送信し、最終的にGrafanaダッシュボードに反映させる手順を紹介しました。

ダッシュボードは作って終わりではなく、ここを起点にさまざまなアラートや自動化を行うことができます。
Grafanaでは閾値を設定した通知機能がありますので、気温が一定の温度を超えたらメールやSlackの通知を行うことができます。

さらに、SwitchBot APIを使えば指ロボットや、電球などの家電製品を自動で操作することができます。
また、上級者向けですが、赤外線コマンドをAPIで操作することで、赤外線リモコンの機能を再現することもできます。
それを活かして、例えば以下のようなことができます。
 * 照度が下がったら、部屋の電気をつける
 * 温度が30°Cを超えたらエアコンをつける

アイディア次第でさまざまな自動化が可能です。
また、Seeeduinoで今回紹介していないようなセンサー、土壌水分量やEC値、PC値を測定できるものを試すのも面白いと思います。

#@# ゴミ置き場
#@# == なぜ Cortex M0+ を選ぶのか

#@# Seeed Studio の販売サイトを見ると、Seeeduino Lotusには、古い ATmega328P 版と Cortex M0+ 版の2種類が存在します。
#@# ATmega 版は5V系センサーとの互換性が高い反面、現在は製造終了となっており、入手が難しくなっています。

#@# 一方で Cortex M0+ 版は 3.3V 動作ですが、
#@# Seeed Studio が現在も出荷している現行モデルであり、Arduino IDE との互換性も高く、今後のセンサーラインナップにも対応しています。
#@# 本書では、再現性と調達性を優先し、Cortex M0+ 版を標準環境として採用します。

#@# == 旧版（ATmega328P版）を使用する場合

#@# もし手元に ATmega328P 版 Seeeduino Lotus がある場合は、
#@# Arduino IDE の [ツール] → [ボード] → [Seeed AVR Boards] → [Seeeduino Lotus] を選択してください。
#@# 接続・スケッチ手順はほぼ同じですが、一部のライブラリで互換性が異なる場合がある点だけ注意すれば問題なく動作します。

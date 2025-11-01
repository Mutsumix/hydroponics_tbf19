= センサーを使ってモニタリングしよう

ここまで、SwitchBotやスマートフォンを使って環境データを集め、ダッシュボードで可視化したり、定点観測カメラで状況を記録する仕組みを作ってきました。
クラウドを中心とした「観測のシステム」はひと通り構築してきたことになります。

次のステップは、データの源そのものを自分の手で扱うことです。
「データを使う」から「データを生み出す」段階に進みます。

ソフトウェアの世界では、監視（モニタリング）は「ツールの運用」ではなく、「どの情報を、どの精度で観測するかを設計すること」だとよく言われます。@<fn>{monitoring}
//footnote[monitoring][Mike Julian 著『入門 監視』でも、この観点が強調されています。]

それはハードウェアでも同じです。
観測の設計をサーバーの外―、現実世界に拡張すると、データの意味や精度、そして自然の中での生まれ方まで意識するようになります。
それはより深いレベルで、生育する植物を理解することにつながります。

この章では、Arduino（アルデュイーノ）互換機である Seeeduino Lotus を使い、水位・水温・照度・騒音といった環境センサーを自分で接続し、実際にデータを取得していきます。
これまで扱ってきた監視の仕組みの基盤となる部分、つまり「データが生まれる層」を、自分の手で組み上げていきましょう。
 
#@# ここで紹介するセンサーは全て用意する必要はありません。まずは自分が必要だと思うものから

== あなたにArduinoを愛していないとは言わせない
Raspberry PiとArduino（およびその互換機であるSeeeduino）は、どちらも小型のコンピュータです。
見た目は似ていますが、内部の仕組みと得意分野はまったく異なります。
この違いを理解しておくと、「なぜArduino系がセンサーに向いているのか」「なぜラズベリーパイが手元にあるのにそれで全部済ませないのか」が自然に見えてきます。

=== 電圧と信号の扱いがシンプル

Arduino系ボードは、センサーを直接つなぐことを前提に設計されています。
多くのセンサーは3.3 Vまたは5 Vで動作し、出力も0〜5 Vのアナログ信号や単純なデジタル信号です。
Arduinoでは、これらの電圧をそのまま安全に扱うことができます。
A0〜A5のピンにアナログセンサーを挿せば、その電圧をanalogRead()で数値として取得できます。

一方で、Raspberry PiのGPIOピンは3.3 V専用です。
5 V信号をそのまま入力すると壊れるおそれがあるため、「レベルシフタ」や「抵抗による分圧回路」などを追加する必要があります。
また、Raspberry Piにはアナログ入力ピンがありません。
水位センサーや温度センサーのようにアナログ出力を持つデバイスを使うには、別途A/D変換ICを配線して接続しなければなりません。

今回扱う、水位・水温・照度・騒音といったセンサーはすべてアナログ出力を持っています。
そのため、Arduino系ボードであれば追加の部品なしで、最短の配線で動作するというわけです。

=== 構造が単純で安定している

Arduino（Seeeduino）とRaspberry Piの構造の最も大きな違いは、OSの有無です。
Arduino系ボードは、**マイクロコントローラ（MCU）** が直接動作する構造になっています。
電源を入れると、ただひとつのプログラムが即座に動き出し、一定間隔でセンサーを読み取るといった処理を確実に実行します。
OSがないため、タイミングのばらつきやプロセス干渉の心配がありません。

同じことをRaspberry Piでやる場合、OSがあるため、OSのセットアップやプログラムの書き込み、自動実行の設定、必要なソフトのインストールなど、いろいろな準備が必要になります。
Arduino系は電源を入れるだけですぐに書き込んだプログラムが動きますが、ラズベリーパイはパソコンに近いので、手間がかかります。
どちらが優れているという話ではなく、センサーを扱うにはArduino系が向いているということです。
そのため、Raspberry Piはデータを集めて解析したり、クラウドに送信したりする側、Arduinoは現場でセンサーを扱う側として使い分けるのが良いでしょう。

=== 配線トラブルを減らせる

センサーを複数扱うと、電源（VCC・GND）や信号線の取り回しで配線が複雑になりがちです。
ジャンパワイヤを使ったブレッドボード配線は学習には良いのですが、接触不良や挿し間違いが起きやすく、初心者にとってはトラブルの原因になりやすい部分です。
初心者がはんだ付けや配線の間違いで詰まってしまった場合、周りに電子工作に詳しい人がいないと、解決ができず、徒労感と共に学習が止まってしまうことがあります。
本書ではそういったことが起きないように、できるだけシンプルな構成でセンサーデータを取得できるような機器を選定しました。@<fn>{soldering}

Seeeduinoシリーズでは、配線問題の解決するためにGroveコネクタが採用されています。
Groveは4ピンの専用ケーブルで、センサーを差し込むだけで接続できる仕組みです。
電源と信号線の配線を自動で正しくまとめてくれるため、間違いを最小限に抑えながら安全に実験できます。

//footnote[soldering][もっと正直に言うと、文系ソフトウェアエンジニアの自分にとって回路図や配線図がマニュアルに出てきた瞬間「自分とは関わりのない世界だ！」と脳が拒絶反応を示して激萎えしてしまいます。そんな自分でもセンサーでデータを取得したい！と言う願望を叶えてくれるのがSeeed社のセンサーたちだったんです。]

=== 互換機 Seeeduino（シーデュイーノ）とは
そしてここで、Arduino互換機のSeeeduinoについて説明します。まず、Arduinoは、イタリアのArduino社が開発したオープンソースのマイコンボードです。「オープンソース」であるため、設計図が公開されており、誰でも互換機を作ることができます。
Seeeduinoは、中国のSeeed Studio社が製造するArduino互換機です。Arduino公式ボードとピン配置や動作は完全に同じなので、Arduinoのスケッチ(プログラム)やライブラリがそのまま使えます。

@<b>{なぜ互換機を選ぶのか}

Arduino公式のUno R3は3,000円程度ですが、Seeeduinoは1,500円程度で購入できます。価格差の理由は、Arduino公式ブランドのライセンス料が含まれていないためです。
互換機を使っても機能的なデメリットはほぼありません。むしろ、Seeeduinoには後述するGroveコネクタという独自の利点があります。

@<b>{Groveコネクタという福音}

通常、Arduinoにセンサーを接続する場合、ブレッドボードとジャンパーワイヤーを使った配線作業が必要です。
センサー → ジャンパーワイヤー数本 → ブレッドボード → Arduino

この方法だと前述の通り、初心者にとって、配線ミスが多発しやすく、見た目が汚くなり、接触不良も起きやすくなります。

@<b>{Groveの仕組み}

@<b>{Grove}は、Seeed Studio社が開発した統一規格のコネクタシステムです。

 - センサー側に4ピンのGroveコネクタが付いている
 - Arduino側にもGroveコネクタ(受け側)が付いている
 - 専用の4芯ケーブル1本で繋ぐだけ


@<code>{センサー → Groveケーブル1本 → Arduino}


**利点:**
1. **配線ミス不可能**: コネクタの形状が決まっているので、間違った挿し方ができない
2. **見た目スッキリ**: ケーブル1本だけなので、配線が整理されている
3. **着脱が簡単**: センサーの交換や追加が数秒で完了

=== Groveエコシステム

Seeed Studioは、Groveコネクタ対応のセンサーモジュールを**300種類以上**販売しています。

 * 温湿度センサー
 * 土壌水分センサー
 * 照度センサー
 * CO2センサー
 * 気圧センサー
 * 水位センサー
 * などなど

すべて同じGroveケーブルで繋げるため、「このセンサーはI2C、こっちはアナログ...」といった接続方式の違いを意識する必要がありません。

=== Seeeduino製品ラインナップ

Seeeduinoには複数のモデルがありますが、本書ではSeeeduino Lotusを使用します。

//table[seeeduino_product_lineup][Seeeduino製品ラインナップ]{
モデル	特徴	価格
------------------
Seeeduino V4.2	Arduino Uno互換の標準機	1,500円
Seeeduino Lotus	Groveコネクタ12個内蔵	2,500円
Seeeduino XIAO	超小型(WiFi対応版もあり)	1,000円
//}

=== Lotus選定の理由

Seeeduino Lotusは、基板上に12個のGroveコネクタが最初から実装されています。

 * デジタルコネクタ × 7個 (D2〜D8)
 * アナログコネクタ × 3個 (A0〜A2)
 * I2Cコネクタ × 2個

これにより、複数のセンサーの配線を拡張ボードなしで、一つの基盤のみでこなすことができます。

=== ここまでのまとめ
ここまでで、
 * Arduino系ボードの特徴
 * SeeeduinoとGroveの利点
 * Seeeduino Lotusを選ぶ理由

が理解できたと思います。次からは、実際にSeeeduino Lotusをセットアップし、最初のセンサー(温湿度センサー)からデータを取得するまでの手順を解説します。

== 購入品一式

//table[sensor_list][購入品一式]{
品目	個数	購入先	目安の価格
------------------
Seeeduino Lotus Cortex M0+@<fn>{seeeduino_lotus}	1つ	Seeed Studio	¥10,800
温度センサー@<fn>{temperature_sensor}	1つ	Seeed Studio	¥1,080
照度センサー@<fn>{light_sensor}	1つ	Seeed Studio	¥1,080
音量センサー@<fn>{volume_sensor}	1つ	Seeed Studio	¥1,080
水位センサー@<fn>{water_level_sensor}	1つ	Seeed Studio	¥1,080
LCD液晶ディスプレイ@<fn>{lcd_display}	1つ	Seeed Studio	¥1,080

//}

//TODO 正しいリンクと価格
//footnote[seeeduino_lotus][@<href>{https://www.seeedstudio.com/Seeeduino-Lotus-p-2517.html} Seeeduino Lotus]
//footnote[temperature_sensor][@<href>{https://www.seeedstudio.com/Grove-Temperature-Sensor-V1.0-p-1142.html} 温度センサー]
//footnote[light_sensor][@<href>{https://www.seeedstudio.com/Grove-Light-Sensor-p-1141.html} 照度センサー]
//footnote[volume_sensor][@<href>{https://www.seeedstudio.com/Grove-Sound-Sensor-p-1000207.html} 音量センサー]
//footnote[water_level_sensor][@<href>{https://www.seeedstudio.com/Grove-Water-Sensor-p-1000206.html} 水位センサー]
//footnote[lcd_display][@<href>{https://www.seeedstudio.com/Grove-LCD-RGB-Backlight-p-2101.html} LCD液晶ディスプレイ]

== センサーの接続方法

センサーの接続方法は解説がいるのか？と疑問に浮かぶくらい非常に簡単です。

Seeeduino Lotusに12個搭載されているGroveコネクタと呼ばれるコネクタに、差し込むだけです。

ただし、Groveコネクタはセンサーの種類によって、差し込むコネクタの位置が異なります。

そのため、次の図を参考に配線を行ってください。

== センサーデータ取得のプログラム作成

それでは、センサーデータを取得するプログラムを作成していきます。

手順は次のとおりです。多いですが、一つ一つ順を追って見ていきましょう。

 1. Arduino IDEの設定をする
 1. GitHubから一定感覚でセンサーデータを取得するプログラムのソースコードを取得する
 1. プログラムをSeeeduino Lotusにアップロードする
 1. 出力結果を確認する
 1. Seeeduino Lotusをラズベリーパイに接続する
 1. ラズベリーパイで、Seeeduino Lotusから取得したセンサーデータをThingSpeakに送信するプログラムを実行する
 1. 結果をThingSpeakで確認する
 1. Grafanaのダッシュボードを更新する

=== Arduino IDEをインストールする

Arduino公式サイトからIDEをダウンロードしてインストールします。  
https://www.arduino.cc/en/software

Windows、macOSいずれでも構いません。インストール後、「ツール」メニューが開けることを確認します。

== Seeeduino用ボードパッケージを追加する

Seeeduino Lotus Cortex M0+ は Arduino 互換機ですが、Arduino Uno とは製造元・チップ構成が異なります。
そのため、Arduino IDE に Seeed 製ボードパッケージを追加する必要があります。

メニューから [ファイル] → [環境設定] を開く

「追加のボードマネージャのURL」欄に次を入力

https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json

［OK］を押して閉じる

=== ボードパッケージのインストール

[ツール] → [ボード] → [ボードマネージャ…] を開く

検索欄に「seeeduino」と入力

「Seeed SAMD Boards」を探して「インストール」をクリック

これで Seeeduino Cortex M0+ 系のボードが Arduino IDE に追加されます。

== Seeeduino Lotus Cortex M0+ をPCに接続する

Seeeduino Lotus Cortex M0+ を Micro USB ケーブルで PC に接続します。
ケーブルは「データ通信対応」のものを必ず使用してください。
100円ショップでよく売られている「充電専用」と書かれたケーブルではスケッチの書き込みができません。

接続が完了すると、Arduino IDE のポートリストに新しいポートが表示されます。
Windows では「COMx」（x は数字）、macOSやLinuxでは「/dev/cu.usbmodemXXXX」といった形式になります。
ポートが分からない場合は、一度 USB を抜き差しして、新たに現れたポートを選びます。

todo 画像

== ボードとポートを設定する

ボードとポート、紛らわしいですが別物です。この場合のポートとは、PCとボードを接続するための通信線のことです。

//TODO LoRa WAN ？結局？ * [ツール] → [ボード] → [Seeed SAMD Boards] → [Seeeduino Lotus Cortex M0+] を選択
 * [ツール] → [ポート] で接続されたポートを選択

ポートが表示されない場合はドライバが必要になる場合があります。
最新のドライバは Seeed Studio の公式Wikiから入手できます。
https://wiki.seeedstudio.com/Seeeduino_Lotus_Cortex_M0+/

ボーレートは 115200bps に設定しておきましょう。

== 接続確認（Lチカ）

動作確認のため、[ファイル] → [スケッチ例] → [01.Basics] → [Blink] を開いて書き込みます。
基板上の LED が 1 秒間隔で点滅すれば成功です。

Seeeduino Lotus Cortex M0+ は Arduino Zero と同じ SAMD21 系チップを使用しており、
USBポートがネイティブUSBとして動作します。
シリアルモニタを開く場合は SerialUSB を使う点に注意してください。

== センサーを接続する

ここから、Seeeduino Lotus Cortex M0+ に複数の環境センサーを接続してデータを取得します。
対象とするセンサーは次の4種類です。

温度センサー（DS18B20）

水位センサー（Grove Water Level）

照度センサー（Grove Light Sensor TSL2561）

騒音センサー（Grove Loudness Sensor）

Cortex M0+ は Grove コネクタが標準搭載されており、ケーブルを差し込むだけで接続が完了します。

センサー	接続ポート
温度センサー	D2
水位センサー	I2C (0x77/0x78)
騒音センサー	A6
照度センサー	I2C

== なぜ Cortex M0+ を選ぶのか

Seeed Studio の販売サイトを見ると、Seeeduino Lotusには、古い ATmega328P 版と Cortex M0+ 版の2種類が存在します。
ATmega 版は5V系センサーとの互換性が高い反面、現在は製造終了となっており、入手が難しくなっています。

一方で Cortex M0+ 版は 3.3V 動作ですが、
Seeed Studio が現在も出荷している現行モデルであり、Arduino IDE との互換性も高く、今後のセンサーラインナップにも対応しています。
本書では、再現性と調達性を優先し、Cortex M0+ 版を標準環境として採用します。

== 旧版（ATmega328P版）を使用する場合

もし手元に ATmega328P 版 Seeeduino Lotus がある場合は、
Arduino IDE の [ツール] → [ボード] → [Seeed AVR Boards] → [Seeeduino Lotus] を選択してください。
接続・スケッチ手順はほぼ同じですが、一部のライブラリで互換性が異なる場合がある点だけ注意すれば問題なく動作します。

=== コード

次のスケッチをArduino IDEで開いて書き込みます。  
各センサーが接続されていない場合でも、エラーを出さずに動作します。

//listnum[sensor_monitor][sensor_monitor.ino]{

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Digital_Light_TSL2561.h>
#include "DFRobot_EC10.h"

#define ONE_WIRE_BUS 2
#define EC_PIN A2
#define LOUDNESS_SENSOR A6
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
DFRobot_EC10 ec;

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

void setup() {
Serial.begin(115200);
Wire.begin();
tempSensor.begin();
TSL2561.init();
ec.begin();
Serial.println("=== Sensor Monitor ===");
}

void loop() {
static float lastValidTemp = 25.0;

tempSensor.requestTemperatures();
float temperature = tempSensor.getTempCByIndex(0);
if(temperature < -50 || temperature > 80) {
temperature = lastValidTemp;
} else {
lastValidTemp = temperature;
}

int waterPercent = getWaterLevelPercent();

int rawEC = analogRead(EC_PIN);
float voltage = rawEC * 3300.0 / 1024.0;
float ecValue = ec.readEC(voltage, temperature);
float ecCalibratedUS = ecValue * 1000 * 0.229;

int loudnessRaw = analogRead(LOUDNESS_SENSOR);
float loudness_db = convertToDB(loudnessRaw);

unsigned long lux = TSL2561.readVisibleLux();

Serial.print("Temp: "); Serial.print(temperature, 1); Serial.print("C | ");
Serial.print("Water: "); Serial.print(waterPercent); Serial.print("% | ");
Serial.print("EC: "); Serial.print(ecCalibratedUS, 0); Serial.print(" uS/cm | ");
Serial.print("Sound: "); Serial.print(loudness_db, 1); Serial.print(" dB | ");
Serial.print("Light: "); Serial.print(lux); Serial.println(" lux");

delay(1000);
}

float convertToDB(int raw) {
if(raw <= 1) return 0.0;
float voltage = raw * (5.0 / 1023.0);
float db = 16.801 * log10(voltage * 10) + 9.872;
if(db < 30) db = 30;
if(db > 90) db = 90;
return db;
}

void getHigh12SectionValue(void) {
memset(high_data, 0, sizeof(high_data));
Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
while (12 != Wire.available());
for (int i = 0; i < 12; i++) {
high_data[i] = Wire.read();
}
delay(10);
}

void getLow8SectionValue(void) {
memset(low_data, 0, sizeof(low_data));
Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
while (8 != Wire.available());
for (int i = 0; i < 8; i++) {
low_data[i] = Wire.read();
}
delay(10);
}

int getWaterLevelPercent(void) {
int low_count = 0;
int high_count = 0;
getLow8SectionValue();
getHigh12SectionValue();
for (int i = 0; i < 8; i++) {
if (low_data[i] > 250 && low_data[i] < 255) low_count++;
}
for (int i = 0; i < 12; i++) {
if (high_data[i] > 250 && high_data[i] < 255) high_count++;
}
int total = low_count + high_count;
return (total * 100) / 20;
}

//}

=== 動作確認

書き込み後、Arduino IDEのシリアルモニタ（115200 bps）を開きます。

=== Sensor Monitor ===
Temp: 24.6C | Water: 80% | EC: 2210 uS/cm | Sound: 45.8 dB | Light: 324 lux
Temp: 24.7C | Water: 79% | EC: 2205 uS/cm | Sound: 47.2 dB | Light: 125 lux

数値が周期的に更新されれば、すべてのセンサーが動作しています。

== 液晶モニタに表示する

I²C接続のLCD（Grove LCD 1602など）をI²Cポートに接続します。  
ライブラリ `<LiquidCrystal_I2C.h>` を利用し、値を表示します。

//listnum[lcd_display][lcd_display.ino]{
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
...
lcd.init();
lcd.backlight();
lcd.print("Sensor Ready");
}

//}

//listnum[lcd_display_loop][lcd_display_loop.ino]{

loop内の最後に次を追加します。

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("T:");
lcd.print(temperature, 1);
lcd.print("C W:");
lcd.print(waterPercent);
lcd.print("%");

lcd.setCursor(0, 1);
lcd.print("L:");
lcd.print(lux);
lcd.print(" S:");
lcd.print((int)loudness_db);

mathematica
コードをコピーする

LCDに温度・水位・照度・音量が更新表示されます。




== Raspberry Piでシリアルデータを受信する

Raspberry PiとSeeeduinoをUSBで接続し、次のコマンドでデバイス名を確認します。

$ ls /dev/ttyACM*

bash
コードをコピーする

一般的には `/dev/ttyACM0` です。  
シリアル受信を確認します。

$ screen /dev/ttyACM0 115200

makefile
コードをコピーする

Pythonでログを取る場合:

import serial
ser = serial.Serial('/dev/ttyACM0', 115200)
while True:
line = ser.readline().decode().strip()
print(line)

bash
コードをコピーする

== ThingSpeakに送信する

ThingSpeakはセンサーデータをHTTPで受け取りグラフ化するWebサービスです。  
https://thingspeak.com/

  #. アカウントを作成し、「New Channel」を追加  
  #. フィールドを5つ設定（Temp、Water、EC、Sound、Light）  
  #. 「Write API Key」を控える  

Raspberry Pi側で次のPythonスクリプトを使います。

import serial, requests

API_KEY = "取得したWriteAPIKey"
ser = serial.Serial('/dev/ttyACM0', 115200)

while True:
line = ser.readline().decode().strip()
data = {}
for kv in line.split('|'):
key, val = kv.strip().split(':', 1)
data[key.strip()] = val.strip().split()[0]
params = {
'api_key': API_KEY,
'field1': data.get('Temp'),
'field2': data.get('Water'),
'field3': data.get('EC'),
'field4': data.get('Sound'),
'field5': data.get('Light')
}
requests.get("https://api.thingspeak.com/update", params=params)

markdown
コードをコピーする

これでThingSpeakに1秒ごとのデータがアップロードされます。

== Grafanaでダッシュボードを作る

Grafanaの「Add Data Source」でHTTP APIを選び、ThingSpeakのチャンネルURLを指定します。

  * URL: https://api.thingspeak.com/channels/CHANNEL_ID/feeds.json  
  * Method: GET  
  * Parameters: api_key=READ_API_KEY  

各フィールドを時系列グラフとして表示し、SwitchBotやカメラのデータと同一ダッシュボードに並べます。  
環境全体の状態を一目で確認できるようになります。

== この章のまとめ

この章では以下を実装しました。

* Seeeduino Lotusの設定と接続  
* 複数センサーからのデータ取得  
* LCDでのローカル表示  
* Raspberry Pi経由のThingSpeak送信  
* Grafanaによる可視化  

これで、環境データの取得からクラウド可視化まで一連の流れが完成しました。

==その他

=== サードパーティ製ボードのインストール

=== Arduino IDEの代わりにPlatformIOを使う

Arduino IDEを使わずに、Visual Studio Codeを使ってプログラムを書く方法を紹介します。
Visual Studio Code 拡張のPlatformIOというものがあります。
今回のようなシンプルなプログラムであれば、Arduino IDEでも十分ですが、自分の好きなエディタを使いたい場合は、PlatformIOをVisual Studio Codeにインストールしてください。@<fn>{platformio}

//footnote[platformio][@<href>{Visual Studio Code 拡張の方が、AIツールによる支援が受けやすいというメリットがあります。もし自分でプログラムを修正したい、という場合は、PlatformIOを使うと良いでしょう。}]

=== さらにやってみよう
 * 閾値を設定した通知機能
	* 例えば温度が30℃を超えたら通知を出す
	* 水位が80%を超えたら通知を出す
	* 照度が1000luxを超えたら通知を出す
	* 騒音が80dBを超えたら通知を出す
 * 他のセンサーを使ってみる
	* 土壌水分量
	* EC値
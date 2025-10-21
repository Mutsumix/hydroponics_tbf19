= 簡単な監視を始めよう

この章では手始めに簡単に購入できる機器を使った監視を紹介します。
プログラミングは必要ありません。そもそもパソコンすら不要です。

== 購入品一式

//table[start_device][購入品一式]{
品目	個数	購入先	目安の価格（単品）
------------------
SwitchBot CO2センサー（温湿度計）@<fn>{switchbot_co2_sensor} 
SwitchBot 屋内カメラ
SwitchBot ハブミニ@<fn>{switchbot_hub_mini}
水温計@<fn>{water_temperature_sensor}
//}

=== 選定理由について

これらを選んだ理由ですが、安さと調達の容易さと、API経由で制御ができる拡張性の高さです。

紹介したSwitchBot の製品はどれも公式サイトやAmazonから購入ができます。

カメラについては、同機能を持つ屋内用ネットワークカメラの中では比較的安い部類に入ります。

また、@<title>{10_dashboard} の章でTODO 後述しますが、

経験上、SwitchBotの製品は初期不良や故障もなくはないのですが、保証期間内であればしっかり対応が受けられます。なんか動きがおかしいな、と思ったらサポートに連絡することをお勧めします。

=== ハブについて

ハブに関しては、温湿度計やスマートリモコン、あるいは照明と一体になった製品も販売されています。

どの製品でも大丈夫なので、ご自宅に必要だと思ったハブ機能を持つ製品を選んでください。ハブに接続することで、遠隔で計測データの確認ができます。

//footnote[smart_plug][@<href>{https://amzn.asia/d/9CN109H} スマートプラグ]
//footnote[switchbot_co2_sensor][@<href>{https://www.switchbot.jp/products/switchbot-co2-meter} SwitchBot CO2センサー（温湿度計）]
//footnote[switchbot_hub_mini][@<href>{https://www.switchbot.jp/products/switchbot-hub-mini} SwitchBot ハブミニ]
//footnote[water_temperature_sensor][@<href>{https://www.switchbot.jp/products/switchbot-water-temperature-meter} SwitchBot 水温計]

== 設置

各製品にケーブルを繋ぎ、SwtichBotのマニュアルに従って初期設定を行い、ハブに接続します。

2025年10月現在の手順では、アプリ→温湿度計グラフページ下にある「データをクラウドに保存」ボタンを手動でタップする必要があります。そうすれば、ハブに接続できが可能になります。


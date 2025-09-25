= センサーを活用したモニタリング

植物の生育状況を把握するためには、まずどんな環境で植物が育っているかを正確に把握し、過去からの変化に注意を払う必要があります。

この章ではCO2センサーを用いた値の取得方法とモニタリングについて解説します。

まずは概要を説明します。

用意する機器はSwitchBotから発売されているCO2センサーと、ハブミニです。CO2センサーはさまざまな製品が市場に出回っていますが、以下の理由からこの２つを選定しました。

 * 現時点で今後も安定的に調達できる見通しがある（レタス編で紹介したセンサーが販売中止になったことを受けて、この点は非常に重視しています）
 * 比較的安価である（安すぎると品質に疑問がつきますが、本書を手にする人には手軽に監視の楽しさと重要性に触れてほしいと思っています）
 * クラウドにデータを飛ばすことができる（自宅でしか監視できませんだと、日中家にいない人や育成環境が自宅にない人にとってあまり意味がありません）
 * データの飛ばし方が容易（レタス編で紹介したセンサーでは、値の取得のためにラズベリーパイを必要とし、さらにセンサーを認識させるためのコマンドの説明が必須で、プログラム初心者にはかなりハードルの高いものだったと反省しています）

センサーの値を取得するために作成するプログラムは、AWSというクラウドサービス上のLambdaというサービスを使って動かします。

そして、取得した値を確認するために、Mackerel（マカレル）という日本のはてな社が提供しているサービスを利用します。

全体の構成図を以下に示します。

//image[monitoring_overview.drawio][全体の構成図]

それでは見ていきましょう。

== 用意する機器

 * SwitchBot CO2センサー（温湿度計）@<fn>{switchbot_co2_sensor}
 * SwitchBot ハブミニ@<fn>{switchbot_hub_mini}

//image[switchbot_co2_sensor][CO2センサー][scale=0.5]
//image[switchbot_hub_mini][ハブミニ][scale=0.5]

//footnote[switchbot_co2_sensor][@<href>{https://www.switchbot.jp/products/switchbot-co2-meter} SwitchBot CO2センサー（温湿度計）]
//footnote[switchbot_hub_mini][@<href>{https://www.switchbot.jp/products/switchbot-hub-mini} SwitchBot ハブミニ（関連商品のハブ2, ハブ3でも可}]

=== CO2センサーを使用するにあたっての初期設定

センサーとハブミニの両方に電力を供給し、電源をつけます。
SwitchBotの公式アプリを使用し、ハブミニにセンサーを認識させます（ハブミニはWi-Fiに接続させておく必要があります）。

詳細な手順は、機器のバージョンによって変わるので、機器購入時のマニュアルやSwitchBotの公式サイトを参照してください。@<fn>{switchbot_manual}

//footnote[switchbot_manual][@<href>{https://support.switch-bot.com/hc/ja/articles/20426539496727-%E6%B8%A9%E6%B9%BF%E5%BA%A6%E8%A8%88%E6%B8%A9%E6%B9%BF%E5%BA%A6%E3%83%87%E3%83%BC%E3%82%BF%E3%81%AE%E9%81%A0%E9%9A%94%E7%A2%BA%E8%AA%8D%E6%96%B9%E6%B3%95-%E3%83%8F%E3%83%96%E3%81%AB%E6%8E%A5%E7%B6%9A%E3%81%99%E3%82%8B%E6%96%B9%E6%B3%95} SwitchBot ハブミニ マニュアル]

CO2センサーは単体でも使用することはできますが、連携させることで、家の外からのモニタリングが可能になります。

SwitchBotの公式アプリ上でCO2濃度と温湿度などが確認できるので、この後紹介するLambdaとかMackerel難しい！って人はこれで終了でも大丈夫です。

ではなぜ、このような外部サービスを使った監視を行うのか、その理由を説明します。

==== 様々な形での通知が可能
 アプリが入っているスマホにプッシュ通知を送ることはできますが、自分以外にも通知をしたい、通知の手段としてメールやチャットサービスを使いたい、といったニーズもあります。
クラウドサービスと連携させることで、これらのニーズに応えることができます。
メールやSlackといったサービスを通して通知をすることが可能になるので、通知の自由度を高めることができます。

==== 思い通りのダッシュボードを作成できる
アプリでの値の確認方法ですが、期間や表示形式はアプリ提供者の方法に従うしかありません。
またアプリで確認すると若干遅い、という点がデメリットです。
Mackerelでダッシュボードを自由に作成しブラウザにブックマークすることで、これらのデメリットを解消することができ、監視の自由度が高まります。

　

以上でセンサーの準備は完了です。上記の理由を読んで、面白そうだなやってみたい、と思った方はこのままこの章を読み進めてください。

== API連携の準備、各種アカウントの作成

プログラムを使ってセンサーの値をモニタリングするためには、API連携の準備が必要です。

主に次の作業を行なっていきます。

 1. SwitchBotの認証用トークンの取得
 1. AWSのアカウント作成
 1. Mackerelのアカウント作成
 1. MackerelのAPIキーの取得

まずはSwitchBotをAPIから操作する方法を説明します。

SwitchBotは、SwitchBot ハブを介してCO2センサーがインターネットに接続されていることを前提としています。

=== SwitchBot 認証用トークンの取得

まずは、API認証用のトークンを取得します。
SwitchBotアプリの 設定 > 開発者向けオプション から確認可能です。
開発者向けオプション は、設定 > 基本データ の アプリバージョン を10回ほどタップすると表示されるようになります。

//image[app_version][アプリバージョンを10回ほどタップ][scale=0.4]
//image[app_version_detail][開発者向けオプションが表示される][scale=0.4]
//image[app_version_detail_2][開発者向けオプションからトークン情報が確認可能][scale=0.4]

==== SwitchBot クラウドサービス設定（現在は不要な設定です）

APIから操作するために、SwitchBotの「クラウドサービス」をオンにしてください。（SwitchBotアプリV9.0から、クラウドサービスの項目自体が削除されていて、手動でオンにする必要がなくなりました）@<fn>{switchbot_cloud_service}

//footnote[switchbot_cloud_service][@<href>{https://blog.switchbot.jp/announcement/switchbot-app-9-0/}, SwitchBot アプリバージョン9.0]

==== SwitchBot デバイスIDの取得

続いて、SwitchBotの deviceId を取得します。
コマンド内のAuthorization には、先ほど開発者向けオプションで確認した認証用トークンを指定してください。

//emlist[curlコマンドでデバイス一覧を取得する][curlコマンドでデバイス一覧を取得する]{
curl --request GET 'https://api.switch-bot.com/v1.0/devices' \
  --header 'Authorization: 認証用トークン' \
  --header 'Content-Type: application/json; charset=utf8'
//}

下記のようなレスポンスが返ってきます。

//emlist[curlコマンドで取得したデバイス一覧の例][curlコマンドで取得したデバイス一覧]{
{
    "statusCode": 100,
    "body": {
        "deviceList": [
            {
                "deviceId": "A1234567890Z",
                "deviceName": "ハブ",
                "deviceType": "Hub Mini",
                "enableCloudService": true,
                "hubDeviceId": "000000000000"
            },
            {
                "deviceId": "A9876543210Z",
                "deviceName": "CO2センサー",
                "deviceType": "MeterPro(CO2)",
                "enableCloudService": true,
                "hubDeviceId": "000000000000"
            }
        ],
        "infraredRemoteList": []
    },
    "message": "success"
}
//}

ここで取得したデバイス一覧の中から、CO2センサーの deviceId を確認してください。この deviceId を後ほどLambdaの環境変数に設定します。

APIドキュメントの詳細はGitHubから確認できます。@<fn>{switchbot_api}

//footnote[switchbot_api][@<href>{https://github.com/OpenWonderLabs/SwitchBotAPI#switchbotapi} SwitchBot API]

=== AWSアカウントの作成

AWSアカウントの作成ですが、こちらについては詳細には説明しません。@<fn>{aws_no_detail}公式のドキュメントを参照してください。@<fn>{aws_account_setup}

//footnote[aws_no_detail][AWSはしょっちゅうUIやボタン位置が変わって、画面のキャプチャを掲載しても数年後には役に立たない、ということが多いからです。]
//footnote[aws_account_setup][@<href>{https://aws.amazon.com/jp/register-flow/} AWSアカウントの作成方法]

途中カード情報の入力が求められます。本書の使用範囲であれば、無料枠で収まるので特に心配はい離ません。ただし、アカウント情報の漏洩には注意してください。

コンソールと呼ばれる設定画面が表示されれば、アカウントの作成は完了です。

//image[aws_console][コンソールの画面][scale=0.75]

=== Mackerelの設定

==== Mackerelのアカウント作成

続いてMackerelのアカウント作成を行います。

まずはMackerelのサインアップ画面にアクセスします。@<fn>{mackerel_signup}

ソーシャルログインが可能なので、GoogleもしくはGitHubアカウントがある方は、そちらでサインアップすると良いでしょう。

//image[mackerel_signup][Mackerelのサインアップ画面][scale=0.75]

//footnote[mackerel_signup][@<href>{https://mackerel.io/signup} Mackerelのサインアップ画面]

オーガニゼーション（組織）名を最初に設定する必要があるので、適当な名前を設定します。個人で利用にチェックを入れて「作成」ボタンを選択します。

//image[mackerel_organization][オーガニゼーションの画面][scale=1.0]

プランを選択する画面が表示されますので、Trialプランを選択します。

//image[mackerel_plan][プランを選択する画面][scale=1.0]

こちらはサインアップ時にカード情報の入力は不要なので、以上でアカウント作成は完了になります。

//image[mackerel_dashboard][ダッシュボードの画面][scale=0.75]

=== MackerelのAPIキーの取得

続いてAPIを取得します。

ダッシュボードから「オーガニゼーション詳細」を選択肢、「APIキー」のタブを選択します。

すでにdefaultのAPIキーが作成されているので、これを使用します。

//image[mackerel_api_key][APIキーの画面][scale=0.75]

名前はなんでも良いのですが、わかりやすいように「編集」ボタンを選択し、「SwitchBot」という名前に変更すると良いでしょう。

//image[mackerel_api_key_edit][APIキーの編集][scale=0.75]

==== Mackerelのサービスの作成

続いて、Mackerelのサービスを作成します。

ダッシュボードから「サービス」を選択し、「サービスを新規作成」ボタンを選択します。

//image[mackerel_service_create][サービスの作成][scale=0.75]

ここではサービス名に「SwitchBotSensor」と入力し、「作成」ボタンを選択します。

//image[mackerel_service_create_2][サービスの作成2][scale=0.75]

これでMackerelに関する準備は完了です。

== センサー値取得プログラムの作成

それでは、CO2センサーから一定間隔でセンター値を取得するプログラムを作成していきます。

手順は次のとおりです。

 1. GitHubから監視プログラムのソースコードを取得する
 1. ソースコード内のzipファイルをAWSのLambdaにアップロードする
 1. 環境変数を設定する
 1. テストを実行し、結果を確認する
 1. AWSのEventBridgeで定期的に実行するように設定する
 1. Mackerelのダッシュボードを作成する

それでは順を追って説明します。

=== GitHubからソースコードの取得

GitHubからプログラムのソースコードを取得します。

著者のリポジトリにアクセスしてください。@<fn>{switchbot_co2_mackerel}

//footnote[switchbot_co2_mackerel][@<href>{https://github.com/Mutsumix/switchbot-co2-mackerel} プログラムのリポジトリ]

アクセスした先の画面で、「Code」ボタンを選択し「Download ZIP」を選択して、zipファイルをダウンロードして、解凍します。

//image[github_download_zip][GitHubからzipファイルをダウンロードする][scale=0.75]
//image[unzip_lambda][zipファイルを解凍する][scale=0.75]

zipファイルを解凍すると、中にlambda.zipというファイルがあるので、これをAWSのLambdaにアップロードします。

使うのはこのzipファイルだけですが、もし内部に興味のある人がいたら、ソースコードも中に入っていますので覗いてみてください。

GitやGitHubに慣れている人はクローンしてください、慣れているあなたには手順なんて不要です。


=== Lambdaの設定

続いてAWSの画面に戻ってLambdaの設定をしていきます。@<fn>{aws_lambda}

//footnote[aws_lambda][@<href>{Lambda（ラムダ）とはAWSのサービスの一つで、サーバーレスコンピューティングサービスです。簡単に言うと、サーバーの管理や設定をすることなく、プログラムコードを実行できるサービスです。必要な時だけ動作し、使った分だけ料金が発生するので、定期的なデータ取得のような用途に最適です。}]

検索画面に「Lambda」と入れて、トップに表示される「Lambda」を選択します。
//image[aws_lambda_search][AWSのLambdaの検索画面][scale=0.75]

==== 関数の作成

「関数の作成」ボタンを選択します。

//image[aws_lambda_create_function][関数の作成画面][scale=0.75]

関数の作成画面で次のように各種設定します。

 * 一から作成を選択
 * 関数名（任意）：switchbot-co2-monitor
 * ランタイム：Python 3.9

 「関数を作成」ボタンを選択します。

//image[aws_lambda_create_function_2][関数の設定画面][scale=0.75]

==== パッケージのアップロード

問題なく関数が作成されると、Lambda関数の設定画面に遷移します。

画面内のコードソースのエリアの右上にある「アップロード元」ボタンを選択し、「.zipファイル」を選択します。

//image[aws_lambda_upload_package][パッケージのアップロード][scale=0.75]

アップロードのポップアップが表示されるので、GitHubからダウンロードしたzipファイルをアップロードし、「保存」ボタンを選択します。

//image[aws_lambda_upload_package_2][GitHubからダウンロードしたzipファイルをアップロードする][scale=0.75]

==== 環境変数の設定

次に、このアップロードしたプログラムが、あなたのSwitchBotの情報を参照し、Mackerelにデータを追加できるように環境変数を設定します。

Lambda関数の設定画面の「環境変数」タブを選択し、「環境変数」の項目を選択し、「編集」ボタンを選択します。

//image[aws_lambda_environment_variables][環境変数の設定][scale=0.75]

環境変数の設定画面で、次のように設定し、「保存」ボタンを選択します。

 * SWITCHBOT_TOKEN：SwitchBotの開発者向けオプションで「トークン」の項目で確認可能
 * SWITCHBOT_SECRET：SwitchBotの開発者向けオプションで「クライアントシークレット」の項目で確認可能
 * SWITCHBOT_DEVICE_ID：Curlコマンドで取得したCO2センサーの deviceId
 * MACKEREL_API_KEY：MackerelのAPIキーの設定画面から確認可能
 * MACKEREL_SERVICE_NAME：SwitchBotSensor

//image[aws_lambda_environment_variables_2][環境変数の設定値][scale=0.75]

==== テストの実行

ここまでできたらテストを実行しましょう

Lambda関数の設定画面の「テスト」タブを選択し、「テスト」ボタンを選択します。

//image[aws_lambda_test][テストの実行][scale=0.75]

画面に「成功」と出たらOKです。

次の手順での確認がしやすいように、もう一度テストを実行しておきましょう。

もしそれ以外の結果が出たら、ここまでの手順を見直してください。

//image[aws_lambda_test_result][テストの実行結果（詳細を広げた状態）][scale=0.75]

結果の確認のためにMackerelの画面も見にいきます。

ダッシュボードから「SwitchBotSensor」を選択し、「サービスメトリック」のタブを選択します。

SwitchBotから取得したデータが送られていることが確認できます。

//image[mackerel_service_metrics][サービスメトリックの画面][scale=0.75]

=== 定期実行の設定

SwitchBotのデータを取得できるようになりましたが、今のままだと、テストを実行した時にしかデータが送られません。

このプログラムが定期的に動くように設定しましょう。

==== AWSのEventBridgeの設定

AWSのEventBridgeを使って、Lambdaを定期的に実行するように設定します。

AWSの検索画面に「EventBridge」と入力して、トップに表示される「Amazon EventBridge」を選択します。

//image[aws_eventbridge_search][AWSのEventBridgeの検索画面][scale=0.75]

左側のメニューから「スケジュール」を選択し、「スケジュールを作成」ボタンを選択します。

//image[aws_eventbridge_create_schedule][スケジュールの作成][scale=0.75]

スケジュールの作成画面で次のように設定します。

 * スケジュール名：switchbot-co2-monitor
 * スケジュールのパターン：定期的なスケジュール
 * スケジュールの種類：rateベースのスケジュール

 30分ごとに実行するように、rate式には次のように設定します。

 * 値：30分
 * 単位：minutes

 フレックスタイムウィンドウは、「オフ」

//image[aws_eventbridge_create_schedule_3][フレックスタイムウィンドウの設定][scale=0.75]

 「次へ」ボタンを選択します。

 ターゲットの詳細を設定する必要があるので、次の通りに設定します。

 * ターゲットの種類：Lambda関数
 * 関数名：switchbot-co2-monitor

//image[aws_eventbridge_create_schedule_4][ターゲットの詳細の設定][scale=0.75]

「次へ」ボタンを選択します。

オプションの設定画面になりますが、特に設定する必要はありません。

再度「次へ」ボタンを選択し、確認画面に遷移し「スケジュールを作成」ボタンを選択します。

以上で、定期実行の設定は完了です。

これで30分に一回、このプログラムが実行される設定が完了しました。

=== Mackerelでダッシュボードを作成する

これでセンサーの値をWeb上で確認できるようになりましたが、せっかくなので、値を見やすいグラフにしてまとめて表示させましょう

筆者はMackerelにこんなダッシュボードを作成しています。
こうすれば、視覚的に今どうなっているのか把握しやすいですし、外出先でも簡単に生育環境の状況を確認することができます。

縦に配置しているのは、スマホから確認した時の可読性を高めるためです。

//image[mackerel_dashboard_2][Mackerelのダッシュボード][scale=0.75]

ダッシュボードを作成する手順を説明します。

左側のメニューから「ダッシュボード」を選択し、「カスタムダッシュボードを作成」ボタンを選択します。

//image[mackerel_dashboard_create][ダッシュボードの作成例][scale=0.75]

任意のダッシュボード名を入力します。

==== Valueのウィジェットの作成

気温のValueウィジェットを作成してみましょう。

Valueのアイコンをドラッグ&ドロップして、ダッシュボードに配置します。

//image[mackerel_dashboard_value_widget][Valueのウィジェットの作成][scale=0.75]

編集画面が開くので、次のように入力します。

 * タイトル：Temperature
 * メトリック：サービスメトリック
 * サービス：SwitchBotSensor
 * メトリック：switchbot.temperature
 * 小数点以下の桁数：1
 * 単位：℃

 正しく設定されれば、編集画面の右側にプレビューが表示されます（@<img>{mackerel_dashboard_value_widget_edit}参照）

//image[mackerel_dashboard_value_widget_edit][Valueのウィジェットの編集][scale=0.75]

==== Graphのウィジェットの作成

次に、CO2のGraphウィジェットを作成してみましょう。

Graphのアイコンをドラッグ&ドロップして、ダッシュボードに配置します。

編集画面が開くので、次のように入力します。

 * タイトル：CO2
 * グラフのタイプ：式グラフ
 * 関数：service(SwitchBotSensor, switchbot.co2)

//image[mackerel_dashboard_graph_widget_edit][Graphのウィジェットの編集][scale=0.75]

=== Mackerelの通知について

Mackerelのメニューにある「監視ルール」を設定することで、ある一定の条件に基づいた通知を行うことができます。

たとえば、CO2が1000ppmを超えたら通知を行う、というような設定ができます。

通知の手段は、メール、Slack、Chatwork、Microsoft Teamsなどから選択できます。@<fn>{mackerel_notification}

//footnote[mackerel_notification][かつてはLINEも選択肢にありましたが、LINE Notifyが2025年3月31日にサービス終了したため、現在は選択することができません]

この辺りは、もし興味があれば、ご自身で調べて設定すると良いでしょう。

//image[mackerel_notification_rule][監視ルールの設定画面][scale=0.75]

=== Mackerelのプランに関する注意点

Mackerelは2週間のTrial期間が終了したら、プランがフリーになりスタンダードの機能が使えなくなります。大きな問題として、データ保存期間が1日のみになります。

もし過去からのセンサー値の推移を把握したいと思う方は月額費用を払って使い続けるとよいでしょう。

== まとめ

ここまでで、SwitchBotのCO2センサーの値をMackerelに送信するところまでを説明しました。

このように、AWSのLambdaを使うことで簡単にセンサーの値を取得し、いつでもどこでも生育環境を確認することができます。

== おまけ（カメラによる監視）

筆者はネットワークカメラにもSwitchBotの製品を使っていますが、ネットワークカメラを使うことで、リアルタイムの生育状況を出先から確認することができます。

//image[switchbot_camera][SwitchBotのネットワークカメラで撮影した画像][scale=0.75]

このカメラで撮った写真を定期的にGooglePhotoに送ることができれば、簡単にタイムラプス動画が作成できるのに、と思っているのですが、うまい方法をまだ見つけられていません。今後の課題です。

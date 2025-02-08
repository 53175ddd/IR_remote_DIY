# IR_remote_recieve

赤外線リモコンの信号デコードプログラム

## とてもくわしいつかいかた

1. Arduino を用意します
2. 赤外線信号受信モジュールを D5 に繋ぎます（プログラムで変更可能）
3. 本プログラムを書き込みます
4. 適当なリモコンを用意します
5. ぽちぽちして遊びます

## 得られる出力

手元にあった ODELIC のリモコンをぽちぽちした結果です

```
Waiting data...

NEC format signal detected
code (BIN) : 00000001-10100011-00010000-11101111
code (HEX) : 0x01A310EF

Waiting data...

NEC format signal detected
code (BIN) : 00000001-10100011-01011001-10100110
code (HEX) : 0x01A359A6
```

## 諸注意

- 現状 NEC フォーマットにしか対応していません（AEHA / SONY フォーマットのリモコンが手元になかったためデバッグできず）
- 本来は LSBFIRST だったりしますが MSBFIRST で受信データを表示するようにしています

コントリビュート待ってます♡

## プロジェクト概要

本アプリケーションは、**NFA（Non-deterministic Finite Automaton）ベースの正規表現エンジン** であり、**壊滅的バックトラックの発生を抑える設計** を採用しています。  
深さ優先探索（DFS）と NFA ベースの正規表現エンジンの共通点を考慮し、DFS における置換表の理論を応用することで、本エンジンは特定の条件下で最適化された処理を実行し、**正規表現の評価時に高いパフォーマンスを実現** します。

また、**「病理的な正規表現」パターンによる壊滅的バックトラックを軽減し、一般的な利用において問題なく動作する設計** を採用しています。

## 構成

```
/regex-project/
├── Linux/ # Linux環境用ファイル
│ ├── 1.sh # "menu.sh" の選択肢 "1" を選んだ時に実行されるスクリプト
│ ├── 2.sh # 同上、"2" 選択肢のスクリプト
│ ├── GNUmakefile # make (g++ / clang++) ビルド用
│ ├── menu.sh # デモアプリを簡単に試せるスクリプト
│ ├── nfa_plus_ttable.cpp # デモアプリのソースコード
│ ├── readme.txt # インストール手順
│ ├── regex.h # 正規表現エンジンのソースコード
├── Windows/ # Windows環境用ファイル
│ ├── 1.bat # "menu.bat" の選択肢 "1" で呼び出されるバッチファイル
│ ├── 2.bat # 同上、"2" 選択肢のバッチファイル
│ ├── makefile # nmake (MSVC) ビルド用
│ ├── menu.bat # デモアプリを簡単に試せるバッチファイル
│ ├── nfa_plus_ttable.cpp # デモアプリのソースコード
│ ├── readme.txt # インストール手順
│ ├── regex.h # 正規表現エンジンのソースコード
```

## 必要環境

- **C++17**（本プロジェクトは C++17 に準拠）
- Windows: `nmake`
- Linux: `GNU make`
- Visual Studio / g++ / clang++で確認済み

## インストール方法

### Windows (MSVC)

```
nmake # "nfa+tt.exe"が作られます
```

### Linux

```
make # "nfa+tt"が作られます
```

## 使い方

### デモの実行

作成したアプリは、スクリプト（バッチファイル）で簡単に動作検証（std::regex とのベンチマークチェック等）を行えます。

#### Windows

```
menu.bat
```

#### Linux

```
./menu.sh
```

### アプリの実行

ビルドしたアプリを実行する方法です。

#### Windows

```
nfa+tt.exe "abaaaaaaaaaaaaaaaaaa" "([ab]+)+b" -time # 対象テキスト、正規表現パターン、オプション（実行時間）
```

出力例：

> **ab**aaaaaaaaaaaaaaaaaa
>
> 0.0711

#### Linux

```
./nfa+tt "abaaaaaaaaaaaaaaaaaa" "([ab]+)+b" -time # 対象テキスト、正規表現パターン、オプション（実行時間）
```

出力例：

> **ab**aaaaaaaaaaaaaaaaaa
>
> 0.0711

### 補足

- 他のコマンドラインオプションは /help または /man オプションで確認できます。

## ライセンス

このプロジェクトは **MIT License** のもとで公開されています。  
詳細は [`LICENSE`](./LICENSE) ファイルをご確認ください。

## 作者情報

本プロジェクトは、**壊滅的バックトラックを抑制する仕組みを正規表現エンジンに実装したデモ用** として作成されました。  
詳しくは `README.md` の説明やソースコードのコメントをご参照ください。

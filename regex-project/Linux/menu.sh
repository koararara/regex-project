#!/bin/bash
clear
echo
echo
echo     ＊ ＊ ＊ ＊ ＊  ＭＥＮＵ  ＊ ＊ ＊ ＊ ＊ 
echo
echo     1. 「各正規表現エンジンとの特性比較」と同じテスト
echo 
echo     2. 「ベンチマーク」と同じテスト
echo
echo     3.  終了
echo
echo
echo
read -p "番号を選んでください．．．" answer
case $answer in
	1 ) ./1.sh ;;
	2 ) ./2.sh ;;
	3 ) clear
	    exit ;;
esac

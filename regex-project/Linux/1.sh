#!/bin/bash
clear
echo
echo
echo 【テスト内容】
echo 「各正規表現エンジンとの特性比較」で取り上げた
echo "「ラス・コックス(Russ Cox)氏の記事」(https://swtch.com/~rsc/regexp/regexp1.html)"
echo  のテストを行う
echo
echo 【測定条件】
echo "測定結果はミリ秒(1000ミリ秒が1秒)単位で表示する"
echo 例外やエラーにより、計測不能になった場合は、「n/a」と表示し、以降の試行は行わない
echo
echo 【測定データ】
echo 正規表現パターン 「'a?' \* n + 'a' \* n」   \(例：n=3\) 「a?a?a?aaa」
echo 対象テキスト     「'a' \* n」              \(例：n=3\) 「aaa」
echo
echo 【測定方法】
echo 文字列長は1から30までは、1文字ずつ増やし、30文字以降は、10文字ずつ増やす。
echo 最長100文字まで増やし、実行時間を計測する。
echo
echo
echo
echo タブ区切りで表示の整形をしているので、場合によっては崩れて表示されることがあります。
echo
echo
echo
read -p "続行するには何かキーを押してください．．．"
clear
echo
echo
echo -e "  n\tNFA+置換表\tノーマルNFA\tstd::regex"
flg1=0
flg2=0
flg3=0
text=
pattern=

function set_case(){
	aq=
	text=
	for((index=1;index<=$1;index++)); do
		aq="${aq}a?"
		text="${text}a"
	done
	pattern="${aq}${text}"
}

function exec_regex(){
	echo -e -n "n = ${1}\t"

	if [ $flg1 -eq 0 ]; then
		./nfa+tt -hide -time "${2}" "${3}" | tr '\012' ' '
		if [ ${PIPESTATUS[0]} -ne 0 ]; then
			flg1=1
		fi
	else
		echo -n "n/a"
	fi
	echo -e -n "\t\t"

	if [ $flg2 -eq 0 ]; then
		./nfa+tt -hide -time "${2}" "${3}" -normal | tr '\012' ' '
		if [ ${PIPESTATUS[0]} -ne 0 ]; then
			flg2=1
		fi
	else
		echo -n "n/a"
	fi
	echo -e -n "\t\t"

	if [ $flg3 -eq 0 ]; then
		./nfa+tt -hide -time "${2}" "${3}" -std | tr '\012' ' '
		if [ ${PIPESTATUS[0]} -ne 0 ]; then
			flg3=1
		fi
	else
		echo -n "n/a"
	fi
	echo 
}

max=30

for ((i=1;i<$max;i++)); do
	set_case ${i}
	exec_regex ${i} ${text} ${pattern}
done

max=100
for ((i=30;i<=$max;i=${i}+10)); do
	set_case ${i}
	exec_regex ${i} ${text} ${pattern}
done
echo
read -p "続行するには何かキーを押してください．．．" answer
./menu.sh


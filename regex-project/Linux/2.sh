#!/bin/bash
clear
echo
echo
echo 【テスト内容】
echo 「ベンチマーク」で取り上げたパターンを実行する
echo
echo 【測定条件】
echo "計測結果はミリ秒(1000ミリ秒が1秒)単位で表示する"
echo 例外やエラーにより、計測不能になった場合は、「n/a」と表示する
echo
echo 【測定方法】
echo 記事では３回測定した内のベストタイムを掲載しているが、今回は一度しか測定しない
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
flg1=0
flg2=0
flg3=0
text=
pattern=
repstr=

function str_repeat(){
	repstr=
	for((index=1;index<=$1;index++)); do
		repstr="${repstr}${2}"
	done
}

function exec_regex(){
	echo -e -n "n = ${1}\t\t"

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


flg1=0
flg2=0
flg3=0
echo
echo "regex : \"(a+)+b\""
echo "text  : 'a' * [n]"
echo 
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i a
	exec_regex ${i} ${repstr} "(a+)+b"
done
echo

flg1=0
flg2=0
flg3=0
echo
echo "regex : \"(\\w|_)+@\""
echo "text  : '_' * [n]"
echo
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i _
	exec_regex ${i} ${repstr} "(\\w|_)+@"
done
echo

flg1=0
flg2=0
flg3=0
echo
echo "regex : \"a(a|aa)*b\""
echo "text  : 'a' * [n]"
echo 
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i a
	exec_regex ${i} ${repstr} "a(a|aa)*b"
done
echo


flg1=0
flg2=0
flg3=0
echo
echo "regex : \"([a-z]+)+$\""
echo "text  : 'a' * [n] + '!'"
echo 
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i a
	text="${repstr}!"
	exec_regex ${i} ${text} "([a-z]+)+$"
done
echo

flg1=0
flg2=0
flg3=0
echo
echo "regex : \".*.*=.*;\""
echo "text  : 'x=' + 'x' * [n]"
echo 
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i x
	text="x=${repstr}"
	exec_regex ${i} ${text} ".*.*=.*;"
done
echo


flg1=0
flg2=0
flg3=0
echo
echo "regex : \"^([\\w+\\-].?)+@[a-z\\d\\-]+(\\.[a-z]+)*\\.[a-z]+$\""
echo "text  : 'username@host.' + 'abcde.' * [n]"
echo 
echo -e "  n\t\tNFA+置換表\tノーマルNFA\tstd::regex"
for i in 10 20 30 100; do
	str_repeat $i abcde.
	text="username@host${repstr}"
	exec_regex ${i} ${text} "^([\\w\\-].?)+@[a-z\\d-]+(\\.[a-z]+)*\\.[a-z]+$"
done
echo
read -p "続行するには何かキーを押してください．．．"
./menu.sh

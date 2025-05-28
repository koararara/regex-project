@echo off
setlocal enabledelayedexpansion
prompt $S
cls

echo.
echo.
echo 【テスト内容】
echo 「各正規表現エンジンとの特性比較」で取り上げた
echo 「ラス・コックス(Russ Cox)氏の記事」(https://swtch.com/~rsc/regexp/regexp1.html)
echo  のテストを行う
echo.
echo 【測定条件】
echo 測定結果はミリ秒(1000ミリ秒が1秒)単位で表示する
echo 例外やエラーにより、計測不能になった場合は、「n/a」と表示し、以降の試行は行わない
echo.
echo 【測定データ】
echo 正規表現パターン 「'a?' * n + 'a' * n」   (例：n=3) 「a?a?a?aaa」
echo 対象テキスト     「'a' * n」              (例：n=3) 「aaa」
echo.
echo 【測定方法】
echo 対象テキストは1から30までは、1文字ずつ増やし、30文字以降は、10文字ずつ増やす。
echo 最長100文字まで増やし、実行時間を計測する。
echo.
echo.
echo.
echo タブ区切りで表示の整形をしているので、場合によっては崩れて表示されることがあります。
echo.
echo.
echo.
pause

cls
echo.
echo  n  		NFA+置換表	ノーマルNFA	std::regex

set v=a
set x=a?
set flg1=0
set flg2=0
set flg3=0

set n=1
set step=1
:loop
	set x=
	set y=
	for /l %%i in (1,1,!n!) do (
		set x=!x!a?
		set y=!y!a
	)
	set p=!x!!y!
rem	echo !p!

	set /P<NUL="n = %n%		"
	if %flg1% equ 0 (
		for /f %%i in ('nfa+tt.exe -time -hide "!y!" "!x!!y!"') do @set tm1=%%i
		set /P<NUL="!tm1!		"
	) else (
		set /P<NUL="n/a		"
	)
	if %flg2% equ 0 (
		for /f %%i in ('nfa+tt.exe -time -hide -normal "!y!" "!x!!y!"') do @set tm2=%%i
		set /P<NUL="!tm2!		"
	) else (
		set /P<NUL="n/a		"
	)
	if %flg3% equ 0 (
		for /f %%i in ('nfa+tt.exe -time -hide -std "!y!" "!x!!y!"') do @set tm3=%%i
		echo !tm3!
	) else (
		echo n/a
	)
	(echo %tm1%) | findstr /l "n/a" > NUL
	if %ERRORLEVEL% equ 0 (
		set flg1=1
	)
	(echo %tm2%) | findstr /l "n/a" > NUL
	if %ERRORLEVEL% equ 0 (
		set flg2=1
	)
	(echo %tm3%) | findstr /l "n/a" > NUL
	if %ERRORLEVEL% equ 0 (
		set flg3=1
	)

	set /a n+=!step!
	if !n! geq 30 (
		set step=10
	)
	if !n! leq 100 goto loop

echo.
pause
menu

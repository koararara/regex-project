@echo off
setlocal enabledelayedexpansion
prompt $S
cls

echo.
echo.
echo �y�e�X�g���e�z
echo �u�e���K�\���G���W���Ƃ̓�����r�v�Ŏ��グ��
echo �u���X�E�R�b�N�X(Russ Cox)���̋L���v(https://swtch.com/~rsc/regexp/regexp1.html)
echo  �̃e�X�g���s��
echo.
echo �y��������z
echo ���茋�ʂ̓~���b(1000�~���b��1�b)�P�ʂŕ\������
echo ��O��G���[�ɂ��A�v���s�\�ɂȂ����ꍇ�́A�un/a�v�ƕ\�����A�ȍ~�̎��s�͍s��Ȃ�
echo.
echo �y����f�[�^�z
echo ���K�\���p�^�[�� �u'a?' * n + 'a' * n�v   (��Fn=3) �ua?a?a?aaa�v
echo �Ώۃe�L�X�g     �u'a' * n�v              (��Fn=3) �uaaa�v
echo.
echo �y������@�z
echo �Ώۃe�L�X�g��1����30�܂ł́A1���������₵�A30�����ȍ~�́A10���������₷�B
echo �Œ�100�����܂ő��₵�A���s���Ԃ��v������B
echo.
echo.
echo.
echo �^�u��؂�ŕ\���̐��`�����Ă���̂ŁA�ꍇ�ɂ���Ă͕���ĕ\������邱�Ƃ�����܂��B
echo.
echo.
echo.
pause

cls
echo.
echo  n  		NFA+�u���\	�m�[�}��NFA	std::regex

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

@echo off
prompt $S
cls
echo.
echo.
echo �y�e�X�g���e�z
echo �u�x���`�}�[�N�v�Ŏ��グ���p�^�[�������s����
echo.
echo �y��������z
echo �v�����ʂ̓~���b(1000�~���b��1�b)�P�ʂŕ\������
echo ��O��G���[�ɂ��A�v���s�\�ɂȂ����ꍇ�́A�un/a�v�ƕ\������
echo.
echo �y������@�z
echo �L���ł͂R�񑪒肵�����̃x�X�g�^�C�����f�ڂ��Ă��邪�A����͈�x�������肵�Ȃ�
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
echo regex  : "(a+)+b"
echo regex  : 'a' * [n]
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "(a+)+b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "(a+)+b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "(a+)+b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "(a+)+b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "(a+)+b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "(a+)+b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "(a+)+b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "(a+)+b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "(a+)+b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "(a+)+b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "(a+)+b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" -hide "(a+)+b" -std') do @set std=%%a
echo %std%
echo.
echo regex  :  "(\\w|_)+@"
echo input  :  '_' * [n]
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide "__________" "(\w|_)+@"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "__________" "(\w|_)+@" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "__________" "(\w|_)+@" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________" "(\w|_)+@"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________" "(\w|_)+@" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________" "(\w|_)+@" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide "______________________________" "(\w|_)+@"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "______________________________" "(\w|_)+@" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "______________________________" "(\w|_)+@" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________________________________________________________________________________________" "(\w|_)+@"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________________________________________________________________________________________" "(\w|_)+@" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "____________________________________________________________________________________________________" "(\w|_)+@" -std') do @set std=%%a
echo %std%
echo.
echo regex  :  "a(a|aa)*b"
echo input  :  'a' * [n]
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "a(a|aa)*b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "a(a|aa)*b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa" "a(a|aa)*b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "a(a|aa)*b" -std') do @set std=%%a
echo %std%
echo.
echo regex  :  "([a-z]+)+$"
echo input  :  'a' * [n] + '!'
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa!" "([a-z]+)+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa!" "([a-z]+)+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaa!" "([a-z]+)+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!" "([a-z]+)+$" -std') do @set std=%%a
echo %std%
echo.
echo regex  :  ".*.*=.*;"
echo input  :  'x=' + 'x' * [n]
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxx" ".*.*=.*;"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxx" ".*.*=.*;" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxx" ".*.*=.*;" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxx" ".*.*=.*;"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide "x=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ".*.*=.*;" -std') do @set std=%%a
echo %std%
echo.
echo regex  :  "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$"
echo input  :  'username@host.' + 'abcde.' * [n]
echo.
echo			NFA+�u���\	�m�[�}��NFA	std::regex
set /P<NUL="n=10		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=20		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=30		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -std') do @set std=%%a
echo %std%
set /P<NUL="n=100		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$"') do @set tt=%%a
set /P<NUL="%tt%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -normal') do @set nfa=%%a
set /P<NUL="%nfa%		"
for /f %%a in ('nfa+tt.exe -time -hide -match "username@host.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde.abcde." "^([\w+\-].?)+@[a-z\d\-]+(\.[a-z]+)*\.[a-z]+$" -std') do @set std=%%a
echo %std%
echo.
pause
menu

exit /b

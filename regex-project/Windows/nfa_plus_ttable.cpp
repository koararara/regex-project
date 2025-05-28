/******************************************************************************
 *                                                                            *
 *  nfa_plus_ttable.cpp                                                       *
 *  Copyright (c) 2020 Gen Inomata. All rights reserved.                      *
 *                                                                            *
 ******************************************************************************/

#include "regex.h"

#ifdef _MSC_VER
#include <Windows.h>
#pragma comment(linker, "/STACK:8388608")    //  スタックサイズ8M
#endif

#include <chrono>
#include <future>
#include <iostream>
#include <optional>
#include <regex>
#include <thread>
#include <tuple>

using namespace std;
using namespace nfa_plus_ttable;

//---------------------------------------------------------------------
//  検索オプション用構造体
//---------------------------------------------------------------------
struct search_options {
    int regex_options = 0;      //  正規表現エンジンへのオプション値
    int timeout       = 10;     //  std::regex実行スレッドのタイムアウト値(秒)
    bool all          = true;   //  反復探索を行うか？
    bool time         = false;  //  実行時間を出力するか？
    bool show         = true;   //  パターンマッチ結果を出力するか？
    bool group        = false;  //  キャプチャした値を表示するか?(showがtrueの場合のみ)
    bool std          = false;  //  std::regexにするか？
};

//---------------------------------------------------------------------
//  カラー表示
//  トグル(toggle変数)で色を変える事で視認性を上げる
//---------------------------------------------------------------------
void color_view(const wstring& s)
{
    static int toggle;
#ifdef _MSC_VER
    //  Windows
    const HANDLE hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    ::GetConsoleScreenBufferInfo(hOut, &csbi);
    constexpr WORD intensity = FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
    constexpr WORD color[] = { FOREGROUND_BLUE | BACKGROUND_GREEN, FOREGROUND_GREEN | BACKGROUND_BLUE };

    ::SetConsoleTextAttribute(hOut, intensity | color[toggle ^= 1]);
    wcout << s;
    ::SetConsoleTextAttribute(hOut, csbi.wAttributes);
#else
    //  Linux
    constexpr wchar_t color[2][12] = { L"\033[1;37;42m", L"\033[1;36;44m" };
    constexpr wchar_t reset[] = L"\033[0m";
    wcout << color[toggle ^= 1] << s << reset;
#endif
}

//---------------------------------------------------------------------
//  使い方を表示
//---------------------------------------------------------------------
int show_usage()
{
    wcout << endl
        << L"使い方 : \"テキスト\" \"正規表現\" (-オプション)" << endl
        << endl
        << L"exec        -  反復探索を行わない" << endl
        << L"group       -  キャプチャされたグループの値を表示する" << endl
        << L"hide        -  実行結果を出力しない" << endl
        << L"icase       -  大文字小文字の区別をしない(アルファベットのみ)" << endl
        << L"limit=秒数  -  タイムアウト時間を秒単位で設定する(std::regexのみ有効)" << endl
        << L"match       -  完全一致検索" << endl
        << L"normal      -  従来型NFAエンジンを使う(置換表を使わない)" << endl
        << L"single      -  単一行探索" << endl
        << L"std         -  C++標準ライブラリ(std::regex)エンジンを使用する" << endl
        << L"time        -  実行時間を表示する" << endl
        << L"?/help/man  -  使い方の表示" << endl
        << endl;
    return 1;
}

//---------------------------------------------------------------------
//  結果の出力
//  一致した箇所を色付きで出力する
//  text    :  検索対象文字列
//  m       :  一致箇所{位置,長さ}。反復探索時は複数個所のデータがある
//  c       :  キャプチャした箇所{位置,長さ}
//---------------------------------------------------------------------
void show_match(wstring text, const vector<pair<ptrdiff_t, size_t>>& m, const vector<vector<pair<intptr_t, size_t>>>& c)
{
    intptr_t seek = 0;                                      //  テキスト位置
    for (const auto& i : m) {
        if (seek < i.first)
            wcout << text.substr(seek, i.first - seek);     //  一致箇所の前方
        seek = i.first;
        if (i.second) {
            color_view(text.substr(seek, i.second));        //  一致箇所をカラー表示
            seek += i.second;
        } else {
            color_view(L" ");                               //  ゼロ幅マッチは空白文字をカラー表示で代用
        }
    }
    if (static_cast<size_t>(seek) < text.length()) {
        wcout << text.substr(seek);                         //  残りのテキスト
    }
    wcout << endl;
    for (size_t i = 0; i < c.size(); i++) {                 //  キャプチャした値を表示する
        if (c[i].size()) {
            wcout << L"match " << i + 1 << endl;
            for (size_t j = 0; j < c[i].size(); j++) {
                wcout << j + 1 << L". " << text.substr(c[i][j].first, c[i][j].second) << endl;
            }
        }
    }
}

//---------------------------------------------------------------------
//  「NFA+置換表」正規表現エンジンでパターンマッチを行う
//  text    :  検索対象文字列
//  pattern :  正規表現パターン文字列
//  opt     :  検索オプション
//  戻り値  :  成功:0, エラー:1, 計測不能:2(opt::timeが有効時のみ)
//---------------------------------------------------------------------
int regex_search_ptt(const wchar_t* text, const wchar_t* pattern, search_options opt)
{
    //  正規表現パターンをコンパイルする
    regex_compiled re(pattern);
    if (re.err_msg().length()) {
        wcout << re.err_msg() << endl;
        return 1;                                                       //  正規表現コンパイルエラーで終了
    }

    regex_ptt regex;
    vector<pair<intptr_t, size_t>> mc;                                  //  一致箇所を格納する{{位置, 長さ}, ...}
    vector<vector<pair<intptr_t, size_t>>> capture;                     //  キャプチャした値を格納する
    auto start = chrono::system_clock::now();                           //  実行時間の計測開始

    //  パターンマッチを実行する
    if (auto res = regex.match(text, re, opt.regex_options)) {
        intptr_t seek = 0;                                              //  探索開始位置
        do {
            if (opt.group)
                capture.push_back({ res.begin() + 1,res.end() });       //  キャプチャした箇所を格納する
            mc.push_back({ res.position(0), res.length(0) });           //  一致箇所を格納する
            seek = res.position(0) + res.length(0);                     //  次の位置
            if (res.length(0) == 0) {                                   //  ゼロ幅マッチ
                if (*(text + seek) == L'\0')
                    break;
                ++seek;                                                 //  ゼロ幅はインクリメントしないと無限ループになる
            }
        } while (opt.all &&
            (res = regex.match(text, re, opt.regex_options, seek)));    //  「反復探索を行う && マッチ成功」がループ条件
    }
    auto end = chrono::system_clock::now();                             //  実行時間の計測終了

    //  結果出力
    int retcode = (regex.error().length() != 0);
    if (opt.show) {
        if (retcode) {
            wcout << regex.error() << endl;
        } else if (mc.size()) {
            show_match(text, mc, capture);
        } else {
            wcout << L"一致しない" << endl;
            wcout << text << endl;
        }
    }

    //  実行時間の出力
    if (opt.time) {
        if (opt.show)
            wcout << endl;
        if (retcode == 0) {
            wcout << chrono::duration_cast<chrono::duration<float, ratio<1, 1000>>>(end - start).count() << endl;
        } else {
            retcode = 2;
            wcout << L"n/a" << endl;                                    //  計測不能
        }
    }

    return retcode;
}

//---------------------------------------------------------------------
//  std::regexでパターンマッチを行う
//  text    :  検索対象文字列
//  pattern :  正規表現パターン文字列
//  opt     :  検索オプション
//  p       :  成功:0, エラー:1, 計測不能:2(opt::timeが有効時のみ)
//---------------------------------------------------------------------
void regex_search_std(const wstring text, const wstring pattern, search_options opt, promise<int> p)
{
    chrono::microseconds ns(0);                     //  実行時間の計測用
    int retcode = 0;                                //  戻り値

    try {
        //  正規表現パターンをコンパイルする
        auto param = std::regex_constants::ECMAScript;
        if (opt.regex_options & regex_ptt::NOCASE)
            param |= std::regex_constants::icase;
        wregex re(pattern, param);

        vector<pair<ptrdiff_t, size_t>> mc;         //  一致箇所を格納する{{位置, 長さ}, ...}
        vector<vector<pair<intptr_t, size_t>>> cp;  //  キャプチャ位置{{位置, 長さ}, ...}
        auto start = chrono::system_clock::now();   //  実行時間の計測開始

        //  パターンマッチを実行する
        if (opt.all && (opt.regex_options & regex_ptt::SEARCH)) {
            //  反復探索を行う
            wsregex_iterator first(text.begin(), text.end(), re);
            wsregex_iterator last;
            for (; first != last; ++first) {
                mc.push_back({ first->position(0), first->length(0) });
                if (opt.group) {
                    vector<pair<intptr_t, size_t>> tmp;
                    for (size_t i = 1; i < first->size(); i++) {
                        tmp.push_back({ first->position(i),first->length(i) });
                    }
                    cp.push_back(tmp);
                }
            }
        } else {
            //  反復探索を行わない
            wcmatch match;
            bool ret = false;
            if (opt.regex_options & regex_ptt::SEARCH) {
                ret = regex_search(text.c_str(), match, re);    //  部分一致探索
            } else {
                ret = regex_match(text.c_str(), match, re);     //  完全一致探索
            }
            if (ret) {
                mc.push_back({ match.position(0),match.length(0) });
            }
            if (opt.group) {
                vector<pair<intptr_t, size_t>> tmp;
                for (size_t i = 1; i < match.size(); i++) {
                    tmp.push_back({ match.position(i), match.length(i) });
                }
                cp.push_back(tmp);
            }
        }

        auto end = chrono::system_clock::now();    //  実行時間の計測終了
        ns = chrono::duration_cast<chrono::microseconds>(end - start);

        //  結果出力
        if (opt.show) {
            if (mc.size()) {
                show_match(text, mc, cp);
            } else {
                wcout << L"一致しない" << endl;
                wcout << text << endl;
            }
        }
    }
    catch (std::regex_error& e) {
        retcode = 1;
        if (opt.show)
            wcout << e.what() << endl;
    }

    //  実行時間の出力
    if (opt.time) {
        if (opt.show)
            wcout << endl;
        if (ns.count() != 0) {
            wcout << chrono::duration_cast<chrono::duration<float, ratio<1, 1000>>>(ns).count() << endl;
        } else {
            retcode = 2;
            wcout << L"n/a" << endl;               //  計測不能
        }
    }
    p.set_value(retcode);
}

//---------------------------------------------------------------------
//  コマンドライン引数を解析する
//  v       :  コマンドライン引数
//  戻り値  :  リターンコード、検索オプション、対象テキスト、パターンを返す
//             リターンコードは、成功時は０、それ以外は非ゼロ
//---------------------------------------------------------------------
tuple<int, search_options, wstring, wstring> parse_args(vector<wstring> v)
{
    //  せっかくなので正規表現で解析を行う
    regex_compiled re(L"[\\/-]((match)|(normal)|(icase)|(single)|(exec)|(time)|(std)|(hide)|(group)|(limit=([1-9]\\d*))|(help|man|\\?))");
    regex_ptt regex;

    search_options opt;
    unsigned int mask = 0;                             //  各オプションとビットを対応させて、オプション指定の有無を判断する
    for (auto it = v.begin(); it != v.end();) {
        if (auto ret = regex.match(it->c_str(), re)) {
            for (size_t i = 0; i < ret.size(); i++) {
                mask |= (ret.length(i) ? (0x01 << i) : 0);
            }
            if (ret.length(12)) {                      //  サブマッチ[12]がタイムアウト値
                opt.timeout = _wtoi(it->substr(ret.position(12)).c_str());
            }
            it = v.erase(it);                          //  合法な値は取り除き、最後に「検索対象文字列」と「パターン文字列」が残るようにする
        } else {
            ++it;
        }
    }
    if (v.size() != 2 || (mask & (0x01 << 13))) {      //  違法パラメータ指定か、(help/man/?)の場合
        return { 1, opt, wstring(), wstring() };
    }

    //  設定したビットを頼りに、検索オプションの値を作る
    opt.regex_options  = ((mask >> 2) & 0x01) ? 0 : regex_ptt::SEARCH;
    opt.regex_options |= ((mask >> 3) & 0x01) ? regex_ptt::NORMAL : 0;
    opt.regex_options |= ((mask >> 4) & 0x01) ? regex_ptt::NOCASE : 0;
    opt.regex_options |= ((mask >> 5) & 0x01) ? regex_ptt::SINGLE : 0;

    //  上と同様に、検索指示フラグを設定する
    opt.all   ^= ((mask >> 6)  & 0x01);     //  exec
    opt.time  ^= ((mask >> 7)  & 0x01);     //  time
    opt.std   ^= ((mask >> 8)  & 0x01);     //  std
    opt.show  ^= ((mask >> 9)  & 0x01);     //  hide
    opt.group ^= ((mask >> 10) & 0x01);     //  group

    return make_tuple(0, opt, v[0], v[1]);
}

//---------------------------------------------------------------------
//  正規表現エンジンへ検索指示する
//  v  :  コマンドライン引数
//---------------------------------------------------------------------
int regex_search(std::vector<wstring> v)
{
    //  コマンドライン引数を解析して検索オプションなどを得る
    auto tupleValue = parse_args(v);
    if (std::get<0>(tupleValue)) {
        return show_usage();
    }

    auto opt = std::get<1>(tupleValue);         //  検索オプション
    auto text = std::get<2>(tupleValue);        //  検索対象テキスト
    auto pattern = std::get<3>(tupleValue);     //  正規表現パターン

    //  検索開始
    if (opt.std) {
        //  std::regex
        //  壊滅的なバックトラックによりフリーズ状態になる可能性があるため
        //  タイムアウト設定をして実行する
        promise<int> p;
        future<int> f = p.get_future();
        thread t(regex_search_std, text.c_str(), pattern.c_str(), opt, move(p));

        auto result = f.wait_for(chrono::seconds(opt.timeout));
        int retcode = 1;
        if (result != future_status::timeout) {
            retcode = f.get();
            t.join();
        } else {
            t.detach();
            if (opt.show)
                wcout << L"time out" << endl;
            if (opt.time)
                wcout << L"n/a" << endl;
        }
        return retcode;
    } else {
        //  NFA+置換表
        return regex_search_ptt(text.c_str(), pattern.c_str(), opt);
    }
}

//---------------------------------------------------------------------
//  コマンドプロンプト(コンソール)で使用している文字コードをUTF-16に変換する
//  Windows APIに習い内部の文字コードは、UTF-16(Unicode)を使用する
//---------------------------------------------------------------------
//  注意：
//  元となる文字コードの判別はしていない
//  Windowsのコマンドプロンプトは、Shift-JISを想定し、
//  Linuxのコンソールは、UTF-8を想定している。
//---------------------------------------------------------------------
optional<wstring> to_utf16(const string& src, unsigned int cp = 932)
{
#ifdef _MSC_VER
    //  Windows
    int len = ::MultiByteToWideChar(cp, 0, src.c_str(), static_cast<int>(src.length()), NULL, 0);
    wstring result(len + 1, L'\0');
    if (::MultiByteToWideChar(cp, 0, src.c_str(), static_cast<int>(src.length()), result.data(), len + 1)) {
        return optional<wstring>{ result };
    }
#else
    //  Linux
    auto len = mbstowcs(nullptr, src.c_str(), 0);
    if (len != static_cast<size_t>(-1)) {
        wstring result(len + 1, L'\0');
        if (mbstowcs(result.data(), src.c_str(), len + 1) != static_cast<size_t>(-1))
            return optional<wstring>{ result };
    }
#endif
    return std::nullopt;
}

//---------------------------------------------------------------------
//  メイン関数
//---------------------------------------------------------------------
int main(int argc, char** argv)
{

#ifdef _MSC_VER
    //  VC++
    wcout.imbue(locale("japanese"));

#ifdef _DEBUG
    //  メモリリーク検出
    ::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

#else
    //  g++/clang++
    setlocale(LC_CTYPE, "ja_JP.UTF-8");
#endif

    if (argc >= 3) {
        vector<wstring> vwstr;
        for (int i = 1; i < argc; i++) {
            if (auto ws = to_utf16({ argv[i] })) {
                vwstr.push_back(ws.value());
            }
        }
        if (vwstr.size() != static_cast<size_t>(argc - 1)) {
            wcout << L"文字コード変換エラー" << endl;
            return 1;
        }
        return regex_search(vwstr);
    }
    return show_usage();
}

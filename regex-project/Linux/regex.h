/******************************************************************************
 *                                                                            *
 *  regex.h                                                                   *
 *  Copyright (c) 2018-2020 Gen Inomata. All rights reserved.                 *
 *                                                                            *
 ******************************************************************************/

#ifndef _REGEX_PLUS_TRANSPOSITION_TABLE_REGEX_H_
#define _REGEX_PLUS_TRANSPOSITION_TABLE_REGEX_H_

#include <wctype.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nfa_plus_ttable
{
#ifndef _MSC_VER
//  Microsoft拡張機能が便利なので、VC++以外の環境用にも用意する
int _wtoi(const wchar_t* ws)
{
    wchar_t* end;
    return (int)wcstol(ws, &end, 10);
}
#endif

/**************************************************************************
 *                                                                        *
 *  ノードのタイプ                                                        *
 *                                                                        *
 **************************************************************************/
enum struct node_type : int {
    DEFAULT = 0,
    END,        //  終了状態
    GROUP,      //  '(' - 開きカッコ
    ENDGROUP,   //  ')' - 閉じカッコ
    CLASS,      //  "[]" - 文字クラス
    ESCAPE,     //  '\\' - エスケープシーケンス
    BOL,        //  '^' - 行頭
    EOL,        //  '$' - 行末
    LOOP,       //  '*' - ループ開始
    ENDLOOP,    //  '*' - ループ終端
};

/**************************************************************************
 *                                                                        *
 *  NFAリンクリストのノード                                               *
 *                                                                        *
 **************************************************************************/
struct nfa_node {
    nfa_node*      n1   = nullptr;              //  遷移先１
    nfa_node*      n2   = nullptr;              //  遷移先２
    const wchar_t* val  = nullptr;              //  正規表現パターン文字列
    intptr_t       len  = 0;                    //  文字列長
    node_type      type = node_type::DEFAULT;   //  識別子(ノードタイプ)
};

/**************************************************************************
 *                                                                        *
 *  コンパイルされた正規表現を管理するクラス                              *
 *                                                                        *
 **************************************************************************/
class regex_compiled
{
public:
    //---------------------------------------------------------------------
    //  コンストラクタ
    //  コンパイルされた正規表現を作成する
    //---------------------------------------------------------------------
    //  regex  :  正規表現パターン文字列
    //---------------------------------------------------------------------
    regex_compiled(const wchar_t* regex) : group_cnt_(0)
    {
        pattern_ = regex;    //  文字列へのポインタを参照するため、コピーを取る
        work_ = pattern_.c_str();
        this->re_ = compile_regex();
    }

    virtual ~regex_compiled()
    {
        if (this->re_)
            clear(this->re_);
    }

    //---------------------------------------------------------------------
    //  内部データにアクセスする為に必要な関数群
    //---------------------------------------------------------------------
    const nfa_node* get() const { return re_; }             //  リンクリストの先頭ノードを返す
    int capture() const { return group_cnt_ + 1; }          //  キャプチャ数。「+1」の意味は"[0]を全体マッチ"で使用するため
    const std::wstring& err_msg() const { return what_; }   //  エラーメッセージを返す

private:
    //---------------------------------------------------------------------
    //  デフォルトコンストラクタなどを使用禁止にする
    //  (正規表現文字列を引数に取るコンストラクタ以外での構築を禁止する目的)
    //---------------------------------------------------------------------
    regex_compiled() = delete;
    regex_compiled(const regex_compiled&) = delete;
    regex_compiled(regex_compiled&&) = delete;
    regex_compiled& operator=(const regex_compiled&) = delete;
    regex_compiled operator=(regex_compiled&&) = delete;

    //---------------------------------------------------------------------
    //  正規表現を内部形式(リンクリスト)にコンパイルする
    //---------------------------------------------------------------------
    //  BNF記法での構文解析ルール(大雑把でアバウトな定義なので、不足分は実装で対処する)
    //  <E> ::= <T> / <E>'|'<T>
    //  <T> ::= <F> / <T><F> / <F>'*' / <F>'+' / <F>'?' / <F>"{n[,m]}"
    //  <F> ::= <C> / <S> / '('<E>')' / '['<C>']'  / '\'<C> / '^' / '$'
    //  <C> ::= 任意の文字
    //  <S> ::= ホワイトスペース
    //---------------------------------------------------------------------
    nfa_node* compile_regex()
    {
        //  メンバ変数 work_ は、パターン文字列の先頭へのポインタ
        const wchar_t* head = work_;    //  構文エラー時に問題箇所を特定するために必要

        if (head == nullptr || *work_ == L'\0') {
            what_ = syntaxerror(nullptr, nullptr);
            return nullptr;
        }

        //  NFAリンクリストの作成
        auto ret = E(new nfa_node());
        ret = cat(ret, new nfa_node({ nullptr, nullptr, nullptr, 0, node_type::END }));    //  「終了状態」

        if (*work_ != L'\0') {
            //  正規表現文字列が最後まで解析されなかった(構文エラー)
            what_ = syntaxerror(head, work_);
            clear(ret);
            return nullptr;
        }
        return ret;
    }

    //---------------------------------------------------------------------
    //  リンクリストのコピー
    //---------------------------------------------------------------------
    nfa_node* copy(const nfa_node* node)
    {
        //
        //  再帰的にコピーを行う(ラムダ式(無名関数)を使用)
        //  「０回以上の繰り返し」の遷移先が「前のノード」に戻る為、
        //  単純にリンクをたどっての再帰では無限ループになる。
        //  unordered_map(mapでも可)を使って「オリジナルnfa_node」と「コピーしたnfa_node」
        //  のペアを持たせて、ループ対策を行っている
        //
        std::unordered_map<const nfa_node*, nfa_node*> hash;
        auto fnc = [&hash](auto f, const nfa_node* n) -> nfa_node* {
            if (!n)
                return nullptr;
            if (hash.count(n))              //  訪問(コピー)済みであるかを確認する
                return hash[n];             //  既に訪問(コピー)済みなので、その時の値を戻す

            auto ret = new nfa_node(*n);    //  コピー作成
            hash[n] = ret;                  //  オリジナルとコピーのペアで登録する。ループチェックに使用する
            ret->n1 = f(f, n->n1);          //  n1遷移側を再帰的にコピーする
            ret->n2 = f(f, n->n2);          //  n2遷移側を再帰的にコピーする
            return ret;                     //  コピーを返す
        };
        return fnc(fnc, node);
    }

    //---------------------------------------------------------------------
    //  リンクリストの末尾ノードを返す
    //---------------------------------------------------------------------
    nfa_node* last(nfa_node* n)
    {
        //  「nfa_complied::copy」同様、ループ対策でunordered_setを使う
        std::unordered_set<nfa_node*> hash;
        hash.insert(nullptr);
        //  n1/n2の両方がnullptr(==末尾)になるまでリンクをたどる
        while (n && (n->n1 || n->n2))
            n = hash.insert(n->n1).second ? n->n1 : (hash.insert(n->n2).second ? n->n2 : nullptr);
        return n;
    }

    //---------------------------------------------------------------------
    //  二つのノードを結合する
    //---------------------------------------------------------------------
    nfa_node* cat(nfa_node* n1, nfa_node* n2)
    {
        if (n1 != nullptr)
            last(n1)->n1 = n2;
        return n1;
    }

    //---------------------------------------------------------------------
    //  nfaリンクリストのノードをvectorに入れて返す
    //---------------------------------------------------------------------
    std::vector<nfa_node*> nfa_list(nfa_node* n)
    {
        //  「nfa_compiled::copy」と同じような処理。ただし、コピーではなく
        //  vectorへ値を格納する
        std::vector<nfa_node*> ret;
        std::unordered_set<nfa_node*> hash;
        auto fnc = [&ret, &hash](auto f, nfa_node* node) -> void {
            if (node && hash.insert(node).second) {
                ret.push_back(node);
                f(f, node->n1);
                f(f, node->n2);
            }
        };
        fnc(fnc, n);
        return ret;
    }

    //---------------------------------------------------------------------
    //  nfaリンクリストのメモリを開放する
    //---------------------------------------------------------------------
    void clear(nfa_node* n)
    {
        //  ループや分岐合流するリンクがあるため、リンクをたどりながら
        //  メモリ解放を行えば、解放済みの不正な領域をアクセスする可能性が
        //  あるため、一旦全ノードを取得してから、メモリ解放をする
        auto list = nfa_list(n);
        for (size_t i = 0; i < list.size(); i++) {
            delete list[i];
            list[i] = nullptr;
        }
    }

    //---------------------------------------------------------------------
    //  通常文字
    //          v
    //  <node>-----><end>
    //----------------------------------------------------------------------
    nfa_node* char1(const wchar_t* v)
    {
        auto end = new nfa_node();
        return new nfa_node({ end, nullptr, v, 1 });
    }

    //---------------------------------------------------------------------
    //  選択
    //    +----<regex1>----+
    //    |                |
    //  <node>           <end>
    //    |                |
    //    +----<regex2>----+
    //---------------------------------------------------------------------
    nfa_node* select1(nfa_node* regex1, nfa_node* regex2)
    {
        auto end = new nfa_node();
        cat(regex1, end);
        cat(regex2, end);

        return new nfa_node({ regex1, regex2 });
    }

    //---------------------------------------------------------------------
    //  ０回以上の繰り返し
    //  (v*)最長一致  (is_lazy == false の時)
    //             ＋-------------＋
    //             ↓             ↑
    //  <node>---><n1>---><v>---><n2>---><end>
    //    ↓                               ↑
    //    ＋-------------------------------＋
    //
    //  (v*?)最短一致  (is_lazy == true の時)
    //  <node>---------------------><end>
    //    ↓                          ↑
    //    ＋---><n1>---><v>---><n2>---＋
    //           ↑             ↓
    //           ＋-------------＋
    //---------------------------------------------------------------------
    nfa_node* star(nfa_node* v, bool is_lazy = false)
    {
        auto end = new nfa_node();
        auto n1 = new nfa_node({ v,nullptr,nullptr,0,node_type::LOOP });
        auto n2 = new nfa_node({ nullptr,nullptr,nullptr,0,node_type::ENDLOOP });
        auto node = new nfa_node();
        if (is_lazy) {  //  最短一致
            n2->n1 = end;
            n2->n2 = n1;
            cat(v, n2);
            node->n1 = end;
            node->n2 = n1;
        } else {        //  最長一致
            n2->n1 = n1;
            n2->n2 = end;
            cat(v, n2);
            node->n1 = n1;
            node->n2 = end;
        }

        //  「(.??)*」の様な正規表現パターンでは「ε遷移」によって無限ループに
        //  なるという問題がある。それを回避するには「ループ間」のテキスト消費を
        //  チェックする。その為にはループ(<n1> - <n2>)範囲を知る必要があるため、
        //  <n1>ノードの「遷移先2」にENDLOOP(<n2>ノード)を設定する
        n1->n2 = n2;

        return node;
    }

    //---------------------------------------------------------------------
    //  １回以上の繰り返し(v+)
    //  v+ == vv*
    //---------------------------------------------------------------------
    nfa_node* plus(nfa_node* v, bool is_lazy = false)
    {
        auto t = copy(v);
        auto s = star(v, is_lazy);
        return cat(t, s);
    }

    //---------------------------------------------------------------------
    //  オプショナル(v?)
    //  v?  == v|ε  (is_lazy == false の時)
    //  v?? == ε|v  (is_lazy == true  の時)
    //---------------------------------------------------------------------
    nfa_node* optional(nfa_node* v, bool is_lazy = false)
    {
        auto e = new nfa_node();    //  ε
        return is_lazy ? select1(e, v) : select1(v, e);
    }

    //---------------------------------------------------------------------
    //  量指定子 (v{n[,m]})
    //  v{2}   == vv        ※mは0に設定
    //  v{2,}  == vvv*      ※mは-1に設定
    //---------------------------------------------------------------------
    nfa_node* range(nfa_node* v, int n, int m, bool is_lazy = false)
    {
        auto t = !n ? new nfa_node() : copy(v); //  nが0の場合はε遷移NFAノードを設定する
        for (int i = 0; i < n - 1; i++) {
            cat(t, copy(v));                    //  v + v + ...(n-1)
        }
        if (!m) {               //  v{n}構文
            clear(v);
            return t;
        } else if (m < 0) {     //  v{n,}構文
            return cat(t, star(v, is_lazy));
        }

        //  v{n,m}構文
        //  
        //  「vvvv|vvv|vv」のようにリンクリストを構築すると、メモリと検索の効率が悪いので
        //  下図のように構築する
        //  
        //  v{2,4}                                     v{2,4}?
        //  -(v)-(v)-<sw>-(v)-<sw>-(v)-<<F>>           -(v)-(v)-<sw>--------<<f>>
        //            |        +---------+                       +-(v)-<sw>---+
        //            +------------------+                              +-(v)-+
        //  
        auto F = new nfa_node();
        nfa_node** c = is_lazy ? &(last(t)->n2) : &(last(t)->n1);
        for (int i = n + 1; i <= m; i++) {
            auto sw = is_lazy ? new nfa_node({ F, copy(v) }) :
                new nfa_node({ copy(v),F });
            *c = sw;
            c = is_lazy ? &(last(sw->n2)->n2) : &(last(sw->n1)->n1);
        }
        *c = F;
        clear(v);
        return t;
    }

    //---------------------------------------------------------------------
    //  <C> ::= 任意の文字
    //  (Unicodeのサロゲートペアなどの処理は複雑になるので実装しない)
    //---------------------------------------------------------------------
    nfa_node* C()
    {
        if (iswprint(*work_))
            return char1(work_++);
        return nullptr;
    }

    //---------------------------------------------------------------------
    //  <S> ::= ホワイトスペース
    //---------------------------------------------------------------------
    nfa_node* S()
    {
        if (iswspace(*work_))
            return char1(work_++);
        return nullptr;
    }

    //---------------------------------------------------------------------
    //  <F> ::= <C> / <S> / '('<E>')' / '['<C>']'  / '\'<C> / '^' / '$'
    //---------------------------------------------------------------------
    nfa_node* F()
    {
        switch (work_[0]) {
        case L'\0':     //
        case L')':      //  定義では「<F>は<C>である」となっていて、<C>は「任意の一文字」と定義されているが、
        case L'*':      //  実際には、ここで「case」分けされている文字は「任意の一文字」から「除外」する必要がある
        case L'+':      //
        case L'?':      //  私のBNF記法での定義は参考程度で「定義漏れ」も多く、不足分は実装時にカバーする必要がある
        case L'|':      //
        case L'{':      //
            return nullptr;
        case L'(': {
            //  グループ、キャプチャ、先読み、後読み
            //  複雑になるので、先読み、後読みは実装しない
            node_type op = node_type::GROUP;
            node_type ed = node_type::ENDGROUP;
            int cnt = 0;
            if (!wcsncmp(work_, L"(?:", 3)) {   //  キャプチャしない指定
                op = ed = node_type::DEFAULT;
                work_ += 2;
            } else {                            //  キャプチャ。簡素化の為「名前付きキャプチャ」は対応しない
                cnt = ++this->group_cnt_;       //  キャプチャの序数
            }
            ++work_;
            auto e = E(new nfa_node());         //  「'('<E>')'」の「<E>」を得る
            if (work_[0] != L')') {
                --work_;
                clear(e);
                return nullptr;
            }
            ++work_;    // ')'分
            auto group = new nfa_node({ e,       nullptr, nullptr, 0, op });    //  '(' <E>
            auto close = new nfa_node({ nullptr, nullptr, nullptr, 0, ed });    //  ')'
            group->len = close->len = cnt;      //  キャプチャの序数(nfa_node::lenメンバ変数を代用している)

            return cat(group, close);           // 「'('<E>」+「')'」
        }
        case L'[': {
            //  文字クラス。'['<C>']'
            intptr_t len = 1;    //  1は'['の分
            //  <C>の部分の文字列長を得る
            len += (work_[len] == L'^') ? 1 : 0;
            len += (work_[len] == L']') ? 1 : 0;    //  '[' or '[^'直後にある']'は文字リテラルとして処理する
            while (*(work_ + len) && work_[len] != L']') {
                if (work_[len] == L'\\')
                    len++;
                len++;
            }
            if (len == 1 || work_[len] != L']')
                return nullptr;    //  構文エラー
            auto ret = new nfa_node({ nullptr, nullptr, work_ + 1, len - 1, node_type::CLASS });
            work_ += (len + 1);
            return ret;
        }
        case L'\\':
            //  エスケープシーケンス。'\\'<C>
            //  複雑になるのでUnicodeプロパティ、8進数・16進数の指定などは、実装しない
            if (work_[1] == L'\0')
                return nullptr;
            work_ += 2;
            return new nfa_node({ nullptr, nullptr, work_ - 2, 2, node_type::ESCAPE });
        case L'^':
            //  行頭
            ++work_;
            return new nfa_node({ nullptr, nullptr, nullptr, 0, node_type::BOL });
        case L'$':
            //  行末
            ++work_;
            return new nfa_node({ nullptr, nullptr, nullptr, 0, node_type::EOL });
        }   //  switch - case 文の終わり

        //  <C> / <S>
        if (auto result = C())
            return result;
        return S();
    }
    //---------------------------------------------------------------------
    //  <T> ::= <F> / <T><F> / <F>'*'  / <F>'+' / <F>'?' / <F>"{n[,m]}"
    //---------------------------------------------------------------------
    nfa_node* T(nfa_node* base)
    {
        if (work_[0] == L'\0')
            return base;

        //  <F>
        auto f = F();
        if (f == nullptr)
            return base;

        bool is_lazy = false;
        switch (*work_++) {
            //  <F>'*' / <F>'+' / <F>'?'
        case L'*':  f = star(f, (is_lazy = *work_ == L'?'));        break;
        case L'+':  f = plus(f, (is_lazy = *work_ == L'?'));        break;
        case L'?':  f = optional(f, (is_lazy = *work_ == L'?'));    break;

            //  <F>{n[,m]}
        case L'{': {
            int n = -1, m = -1;
            const wchar_t* r = work_;
            if (iswdigit(*r)) {
                n = _wtoi(r);
                while (iswdigit(*r))
                    ++r;
                if (*r == L'}') {
                    m = 0;  //  <F>{n}
                } else if (*r == L',' && *(r + 1) == L'}') {
                    m = -1; //  <F>{n,}
                } else if (*r != L',' || ((m = _wtoi(r + 1)) < n)) {
                    n = m = -1;
                }
            }
            if (n == -1) {
                clear(f);
                return base;
            }
            while (*work_++ != L'}');
            f = range(f, n, m, (is_lazy = *work_ == L'?'));
            break;
        }
        default:
            --work_;
        }
        if (is_lazy)
            ++work_;    //  '?'分
        //  <T><F>
        return T(cat(base, f));
    }
    //---------------------------------------------------------------------
    //  <E> ::= <T> / <E>'|'<T>
    //---------------------------------------------------------------------
    nfa_node* E(nfa_node* base)
    {
        auto e = T(base);                   //  <E> ::= <T>
        while (*work_ == L'|') {
            ++work_;
            auto t = T(new nfa_node());     //  <T>
            e = select1(e, t);              //  <E> ::= <E>'|'<T>
        }

        if (e)
            return cat(e, new nfa_node());  //  終端を追加する
        return nullptr;
    }

    //---------------------------------------------------------------------
    //  構文エラー文を作成する
    //---------------------------------------------------------------------
    static std::wstring syntaxerror(const wchar_t* head, const wchar_t* regex)
    {
        std::wstring msg;
        if (!head || !regex) {
            msg = L"\nstring is null or empty.\n";
        } else {
            intptr_t len = (regex - head);
            msg = L"\nsyntax error:";
            std::wstring sp(len + msg.length(), L' ');
            msg += head;
            msg += L"\n";
            msg += sp;
            msg += L"^\n";
        }
        return msg;
    }

private:
    std::wstring   pattern_;                //  正規表現文字列のコピーを保持する(ポインタを使用するためクラス生存期間中は必要)
    const wchar_t* work_      = nullptr;    //  構文解析時に「パターン文字列(pattern_)」を参照する為に使用する
    nfa_node*      re_        = nullptr;    //  リンクリストの先頭ノード
    int            group_cnt_ = 0;          //  グループの数。regex_pttクラスでデータを格納する変数のサイズ計算に必要
    std::wstring   what_;                   //  エラーメッセージ

};

/**************************************************************************
 *                                                                        *
 *  正規表現パターンマッチ結果を管理するクラス                            *
 *                                                                        *
 **************************************************************************/
class regex_result
{
public:
    regex_result() {}
    explicit operator bool() const { return match_.size() != 0; }
    void set(const std::vector<std::pair<intptr_t, size_t>>& v) {
        match_.resize(v.size(), std::pair<intptr_t, size_t>(0, 0));
        for (size_t i = 0; i < v.size(); i++)
            if (v[i].first >= 0 && v[i].second >= 0)
                match_[i] = v[i];
    }
    void set(const std::wstring& err) {
        what_ = err;
    }
    size_t size() const { return match_.size(); }
    intptr_t position(size_t i) const { return match_[i].first; }
    size_t length(size_t i) const { return match_[i].second; }
    bool is_error() const { return what_.length() != 0; }
    const std::wstring& err() const { return what_; }

    //
    //  「範囲for」用
    //
    using iterator       = std::vector<std::pair<intptr_t, size_t>>::iterator;
    using const_iterator = std::vector<std::pair<intptr_t, size_t>>::const_iterator;
    iterator begin() { return match_.begin(); }
    const_iterator cbegin() const { return match_.begin(); }
    iterator end() { return match_.end(); }
    const_iterator cend() const { return match_.cend(); }

private:
    std::vector<std::pair<intptr_t, size_t>> match_;
    std::wstring                             what_;
};

/**************************************************************************
 *                                                                        *
 *  NFA正規表現+置換表エンジン                                            *
 *                                                                        *
 **************************************************************************/
class regex_ptt
{
public:
    static constexpr unsigned int SEARCH = 0x01;        //  検索オプション値 - 部分一致
    static constexpr unsigned int SINGLE = 0x02;        //  検索オプション値 - 「^」が改行の次にマッチしない
    static constexpr unsigned int NOCASE = 0x04;        //  検索オプション値 - 大文字小文字の区別をしない(アルファベットのみ)
    static constexpr unsigned int NORMAL = 0x08;        //  検索オプション値 - 従来型NFAエンジンモード
#ifdef _DEBUG
    static constexpr int64_t      MAX_LIMIT = 100000LL; //  関数呼び出し回数の制限値(長考対策)
    static constexpr long         MAX_DEPTH = 3000L;    //  再帰呼び出し深度の制限値(スタックオーバーフロー対策)
#else
    static constexpr int64_t      MAX_LIMIT = 100000000LL;  //  関数呼び出し回数の制限値(長考対策)
    static constexpr long         MAX_DEPTH = 10000L;       //  再帰呼び出し深度の制限値(スタックオーバーフロー対策)
#endif

    //---------------------------------------------------------------------
    //  コンパイルされた正規表現を受け取り、テキスト内の検索を行う
    //---------------------------------------------------------------------
    //  text    :  検索対象の文字列
    //  re      :  コンパイルされた正規表現オブジェクト
    //  option  :  探索オプション
    //  seek    :  textの検索開始オフセット値
    //  戻り値  :  結果を管理するクラスオブジェクト(regex_result)を返す
    //---------------------------------------------------------------------
    regex_result match(const wchar_t* text, const regex_compiled& re, const int options = 0, const intptr_t seek = 0)
    {
        regex_result result;
        const nfa_node* nfa = re.get();
        if (nfa == nullptr)
            return result;

        what_.clear();                  //  エラー出力メッセージの初期化
        input_head_ = text;             //  検索対象テキストの先頭位置を保存しておく
        if (wcslen(text) < (size_t)seek) {
            runtimeerror(L"buffer overrun detected.");
            return result;
        }
        text += seek;

        //  キャプチャの初期化
        capture_.clear();
        capture_.resize(re.capture(), std::pair<intptr_t, intptr_t>(-1, -1));

        //  置換表のセットアップ
        if (options & regex_ptt::NORMAL) {
            table_ = nullptr;       //  nullptrを設定すれば、置換表を使わない従来型NFAエンジンになる
        } else {
            table_ = &hash_table_;  //  置換表を設定
        }
        table_clear();              //  置換表を初期化する

        //  パターンマッチを開始する
        //  regex_ptt::SEARCH指示の場合は、検索対象テキストの位置を動かしながらパターンマッチ処理を行う
        const wchar_t* ret = nullptr;
        do {
            this->limit_ = regex_ptt::MAX_LIMIT;
            ret = reg_find(nfa, text, regex_ptt::MAX_DEPTH, options);
            if (ret || *text == L'\0' || !what_.empty())
                break;              //  マッチ or テキスト末尾 or error
            ++text;
            if (*text == L'\n') {
                table_clear();      //  置換表容量爆発対策
            }
        } while (options & regex_ptt::SEARCH);

        if (what_.empty() == false) {
            //  エラーメッセージを設定する
            result.set(what_);
        } else if (ret) {
            //  キャプチャ変数の0番目にマッチした全体を登録する
            capture_[0].first = (text - input_head_);       //  マッチ位置
            capture_[0].second = (ret - input_head_);       //  マッチ末尾
            capture_[0].second -= capture_[0].first;        //  文字列長
            result.set(capture_);
        }
        return result;
    }

    //---------------------------------------------------------------------
    //  エラーメッセージを返す
    //---------------------------------------------------------------------
    const std::wstring& error() const
    {
        return what_;
    }

private:
    //---------------------------------------------------------------------
    //  std::unordered_setの第三パラメータに必要なハッシュ関数オブジェクト
    //---------------------------------------------------------------------
    struct hash {
        std::size_t operator()(const std::pair<const nfa_node*, const wchar_t*>& key) const {
            return static_cast<size_t>(rand(reinterpret_cast<intptr_t>(key.first), reinterpret_cast<intptr_t>(key.second)));
        }
        //  xorshift疑似乱数生成アルゴリズム
        uint32_t rand(intptr_t a, intptr_t b) const {
#if defined(_WIN64) || defined(__x86_64__)
            uint32_t x = (uint32_t)((a >> 31) ^ a);
            uint32_t w = (uint32_t)((b >> 31) ^ b);
            uint32_t t = x ^ (x << 11);
            return (w ^ (w >> 19)) ^ (t ^ (t >> 8));
#else
            uint32_t t = a ^ (a << 11);
            return (b ^ (b >> 19)) ^ (t ^ (t >> 8));
#endif
        }
    };

    //---------------------------------------------------------------------
    //  メンバ変数
    //---------------------------------------------------------------------
    using Capture  = std::vector<std::pair<intptr_t, size_t>>;      //  キャプチャ
    using hash_key = std::pair<const nfa_node*, const wchar_t*>;    //  キー
    using Table    = std::unordered_set<hash_key, hash>;            //  置換表

    const wchar_t* input_head_ = nullptr;   //  対象文字列の開始アドレス
    long long      limit_;                  //  バックトラック回数制限用
    std::wstring   what_;                   //  エラーメッセージ
    Capture        capture_;                //  キャプチャ
    Table          hash_table_;             //  置換表
    Table*         table_ = nullptr;        //  置換表(On = &hash_table, Off = nullptr)

private:
    //---------------------------------------------------------------------
    //  再帰的にパターンマッチを行う
    //---------------------------------------------------------------------
    //  node    :  NFAリンクリスト
    //  text    :  検索対象文字列
    //  depth   :  探索深度(スタックオーバーフロー対策)
    //  option  :  探索オプション
    //             SEARCH  -  部分一致探索指示
    //             SINGLE  -  「^」が改行の次にはマッチしない
    //             NOCASE  -  大文字小文字の区別をしない(アルファベットのみ)
    //             NORMAL  -  置換表を使用しない(従来型NFAエンジンモード)
    //  戻り値  :  失敗時はnullptrを返す。成功時はマッチした末尾の位置を返す
    //---------------------------------------------------------------------
    const wchar_t* reg_find(const nfa_node* node, const wchar_t* text, const long depth, const int option)
    {
        if (limit_-- < 0LL || depth < 0L) {
            runtimeerror(L"backtrack limit error.");
            return nullptr;
        }

        if (node == nullptr)
            return nullptr;

        if (node->type == node_type::END) {                             //  NFAリンクリスト終端
            if (option & regex_ptt::SEARCH)
                return text;                                            //  部分一致
            return text[0] == L'\0' ? text : nullptr;                   //  完全一致か不一致
        }

        //  ε遷移無限ループ対策
        //  「置換表」処理の前に行わなければ、「置換表」が誤判定を起こす原因になる
        if (node->type == node_type::ENDLOOP && node->val == text) {    //  ループ間でテキストを消費していない
            if (node->n2->type == node_type::LOOP)                      //  ループせず次へ遷移する
                return reg_find(node->n1, text, depth - 1, option);     //  最短一致の場合はn1が次の遷移先
            return reg_find(node->n2, text, depth - 1, option);         //  最長一致の場合はn2が次の遷移先
        }

        //  置換表(「壊滅的なバックトラック」を抑制する)
        if (table_ && node->n2) {                                       //  置換表が"有効" かつ 分岐のあるノード
            if (table_->insert({ node,text }).second == false)
                return nullptr;                                         //  既に評価済み(「一致しない」を返す)
        }

        intptr_t seek = 0;
        switch (node->type) {
        case node_type::BOL:
            //  「^」- 行頭
            if (text != input_head_ && (option & regex_ptt::SINGLE || *(text - 1) != L'\n'))
                return nullptr;
            break;

        case node_type::EOL:
            //  「$」- 行末
            if (*text != L'\0')
                return nullptr;
            break;

        case node_type::GROUP:
            //  「(」- グループ、キャプチャ
            return group(node, text, depth, option);

        case node_type::ENDGROUP:
            //  「)」- 閉じカッコ
            return end_group(node, text, depth, option);

        case node_type::LOOP:
            //  「*」- ループ開始ノード
            return loop(node, text, depth, option);

        case node_type::CLASS:
            //  「[] or [^]」- 文字クラス
            if (*text == L'\0' || (seek = char_class(node, text, option)) == 0)
                return nullptr;
            break;

        case node_type::ESCAPE:
            //  「\」- エスケープシーケンス
            if ((seek = escape(node->val + 1, text, option)) == -1)
                return nullptr;
            break;

        default:
            if (node->len == 1 && node->type == node_type::DEFAULT) {
                // 「通常文字」
                if ((text[0] == L'\0') || (node->val == nullptr) || (seek = cmp_char(node->val, text, option)) == 0)
                    return nullptr;
            }
        }   //  switch-caseの終了

        //  再帰的に次のノードの探索を続ける
        //  最初にn1遷移を試し、エラー応答でバックトラックしてきたら、(あれば)n2遷移を試す
        auto ret = reg_find(node->n1, text + seek, depth - 1, option);
        if (!ret && node->n2)
            ret = reg_find(node->n2, text + seek, depth - 1, option);
        return ret;
    }

    //---------------------------------------------------------------------
    //  キャプチャの開始処理を行う
    //---------------------------------------------------------------------
    const wchar_t* group(const nfa_node* node, const wchar_t* text, const long depth, const int option)
    {
        auto rollback = capture_[node->len].first;          //  バックトラックしたときに値を戻すために保存する
        capture_[node->len].first = (text - input_head_);   //  キャプチャ開始インデックス値

        auto ret = reg_find(node->n1, text, depth - 1, option);
        if (ret == nullptr) {
            capture_[node->len].first = rollback;           //  失敗で戻る前にロールバックする
        }
        return ret;
    }

    //---------------------------------------------------------------------
    //  キャプチャデータを確定する
    //---------------------------------------------------------------------
    const wchar_t* end_group(const nfa_node* node, const wchar_t* text, const long depth, const int option)
    {
        auto rollback = capture_[node->len].second;     //  バックトラック時に値を戻すために保存する
        capture_[node->len].second = (text - input_head_) - capture_[node->len].first;  //  文字列長

        auto ret = reg_find(node->n1, text, depth - 1, option);
        if (ret == nullptr) {
            capture_[node->len].second = rollback;      //  失敗で戻る前にロールバックする
        }
        return ret;
    }

    //---------------------------------------------------------------------
    //  ε遷移無限ループ対策
    //  LOOP - ENDLOOP間でテキスト消費を監視する
    //---------------------------------------------------------------------
    const wchar_t* loop(const nfa_node* node, const wchar_t* text, const long depth, const int option)
    {
        auto rollback = node->n2->val;  //  node->n2はENDLOOPノード
        node->n2->val = text;           //  ENDLOOP側に現在のテキスト位置を知らせる
        auto ret = reg_find(node->n1, text, depth - 1, option);
        node->n2->val = rollback;       //  バックトラックにより戻ってきたのでロールバックする
        return ret;
    }

    //---------------------------------------------------------------------
    //  「通常文字」の一致処理
    //  複雑になるのでUnicodeのサロゲートペアなどは考慮しない
    //---------------------------------------------------------------------
    int cmp_char(const wchar_t* pattern, const wchar_t* text, const int option) const
    {
        if (*pattern == L'.' && *text != L'\n') {
            return 1;
        }
        if (*pattern == *text)
            return 1;
        if (option & regex_ptt::NOCASE)
            return cmp_nocase(*text, *pattern);
        return 0;
    }

    //---------------------------------------------------------------------
    //  「通常文字」の一致処理を大文字小文字の区別なく行う
    //  簡単にするためアルファベットのみを対象とする
    //---------------------------------------------------------------------
    int cmp_nocase(const wchar_t t, const wchar_t ch) const
    {
        return (t <= L'Z') ? (ch == (L'a' + (t - L'A'))) : (ch == (L'A' + (t - L'a')));
    }

    //---------------------------------------------------------------------
    //  文字クラスの範囲指定(「[a-z]など」)の一致処理を
    //  大文字小文字の区別なく行う
    //  cmp_nocase同様、アルファベットのみを対象とする
    //---------------------------------------------------------------------
    int cmp_nocase(const wchar_t start, const wchar_t end, const wchar_t target) const
    {
        if (iswalpha(target) && ((end >= L'A') && (start <= L'z'))) {
            int tmp = (target <= L'Z') ? (L'a' + (target - L'A')) : (L'A' + (target - L'a'));
            return (start <= tmp && tmp <= end);
        }
        return 0;
    }

    //---------------------------------------------------------------------
    //  文字クラス([], [^])内にある文字と比較する
    //  一致した場合は1を返す。不一致なら0を返す
    //---------------------------------------------------------------------
    int char_class(const nfa_node* node, const wchar_t* text, const int option)
    {
        const wchar_t* s = node->val;
        const int r = (s[0] == L'^');
        for (intptr_t i = r; i < node->len; i++) {
            if (s[i] == L'\\') {
                if (escape(s + (++i), text, option) != -1)
                    return 1 ^ r;
            } else {
                if (s[i + 1] == L'-' && s[i + 2] != L']') {
                    wchar_t start = s[i];
                    wchar_t end = s[i + 2];
                    if (start <= *text && *text <= end)
                        return 1 ^ r;
                    if (option & regex_ptt::NOCASE && cmp_nocase(start, end, *text))
                        return 1 ^ r;
                    i += 2;
                } else if (s[i] == L'.') {
                    if (s[i] == *text)
                        return 1 ^ r;
                } else if (cmp_char(s + i, text, option)) {
                    return 1 ^ r;
                }
            }
        }
        return r;
    }

    //---------------------------------------------------------------------
    //  エスケープシーケンス
    //  戻り値  :  失敗なら-1を返す。成功ならマッチした文字数を返す
    //---------------------------------------------------------------------
    intptr_t escape(const wchar_t* pattern, const wchar_t* text, const int option)
    {
        intptr_t val = -1;
        switch (pattern[0]) {
        case L't':  val = (L'\t' == *text);                             break;  //  水平タブ
        case L'n':  val = (L'\n' == *text);                             break;  //  改行
        case L'r':  val = (L'\r' == *text);                             break;  //  キャリッジリターン
        case L'd':  val = iswdigit(*text);                              break;  //  数字
        case L'D':  val = *text && (escape(L"d", text, option) == -1);  break;  //  数字以外
        case L's':  val = iswspace(*text);                              break;  //  ホワイトスペース
        case L'S':  val = *text && (escape(L"s", text, option) == -1);  break;  //  ホワイトスペース以外
        case L'w':  val = (iswalnum(*text) || L'_' == *text);           break;  //  アルファベットとアンダースコア
        case L'W':  val = *text && (escape(L"w", text, option) == -1);  break;  //  アルファベットとアンダースコア以外
        case L'.':  val = (text[0] == L'.');                            break;  //  文字リテラル「.」
        case L'B':  return !escape(L"b", text, option) ? -1 : 0;                //  単語境界以外
        case L'b':                                                              //  単語境界
            if (escape(L"w", text, option) != -1)
                return (text == input_head_ || escape(L"W", text - 1, option) != -1) ? 0 : -1;
            return (text != input_head_ && escape(L"w", text - 1, option) != -1) ? 0 : -1;
        }   //  switch-caseの終端
        if (val != -1)
            return val == 0 ? -1 : 1;

        if (iswdigit(pattern[0])) {
            //  パターン内後方参照
            int p = _wtoi(pattern);
            if (p >= 1 && p < static_cast<int>(capture_.size()) && capture_[p].second >= 0) {
                if (!capture_[p].second || !wcsncmp(text, input_head_ + capture_[p].first, capture_[p].second))
                    return capture_[p].second;

                //  パターン内後方参照で失敗した場合は、置換表が原因で正しい解が得られない場合があるため
                //  置換表をクリアする必要がある
                table_clear();
            }
            return -1;
        }

        if (int n = cmp_char(pattern, text, option))
            return n;

        return -1;
    }

    //---------------------------------------------------------------------
    //  正規表現探索時エラーメッセージの設定
    //---------------------------------------------------------------------
    void runtimeerror(const std::wstring msg)
    {
        what_ = msg;
    }

    //---------------------------------------------------------------------
    //  置換表を初期化する
    //---------------------------------------------------------------------
    void table_clear()
    {
        if (table_)
            table_->clear();
    }
};
}   //  namespace nfa_plus_ttable
#endif  //  _REGEX_PLUS_TRANSPOSITION_TABLE_REGEX_H_

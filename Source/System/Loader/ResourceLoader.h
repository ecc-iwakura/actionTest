#pragma once
#include <iostream>
#include <Windows.h>
#include <fstream>
//#include "../resource.h"

// EXEに内包されたデータを std::ifstream と同じ様な記述で使用できるようにしたクラス
class ResourceLoader
{
private:
	// 読み込んだバイナリリソースのデータ
	// スマートポインタで保持するとカーソル位置の設定が冗長になるため使用しない
	char* data;

	// 読み込んだバイナリリソースデータのサイズ
	size_t size;

	// リソースを開いているか
	bool isOpen;

	//	読み込み位置(カーソル)
	size_t cursor;

public:
	enum class SeekMode
	{
		Begin,
		End,
	};

	ResourceLoader() :data(nullptr), size(0), isOpen(false), cursor(0) {}
	// 引数で渡された値を Open 関数で使用して読み込む
	ResourceLoader(int resourceIndex, const wchar_t* fileType);
	~ResourceLoader() {}

	// EXEに内包したリソースデータを開く関数
	// 成功すると "true" 失敗すると "false" が返される
	// resourceIndex には resource.h に #define されている名称を入れる
	// fileType には追加したリソースの種類の名前を L"種類名" という形で入れる
	bool Open(int resourceIndex, const wchar_t* fileType);

	// 読み込み位置(カーソル)から loadSize の文字数分 out に返す
	// 読み込んだ分だけカーソルを移動させる
	void Read(char* out, size_t loadSize);

	// 読み込み位置(カーソル)から改行コードもしくは searchChar で指定した文字までの文字列を out に返す
	// 読み込んだ分だけカーソルを移動させる
	void GetLine(char* out, char searchChar = '\n');

	// カーソル位置の移動
	void Seek(SeekMode mode);

	// 現在のカーソル位置を取得
	size_t Tell();

	// 読み込んだリソースデータのサイズを取得
	size_t GetSize();
};


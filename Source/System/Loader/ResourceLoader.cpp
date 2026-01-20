#include <string.h>
#include "ResourceLoader.h"

using namespace std;

ResourceLoader::ResourceLoader(int resourceIndex, const wchar_t* fileType) :data(nullptr), size(0), isOpen(false), cursor(0)
{
	Open(resourceIndex, fileType);
}

bool ResourceLoader::Open(int resourceIndex, const wchar_t* fileType)
{
	// EXEに内包されたリソースの先頭アドレスを探す
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(resourceIndex), fileType);
	if (hResource == NULL) {
		// リソースがなかった場合、false を返す
		OutputDebugString(L"指定されたリソースの検索に失敗しました。");
		return false;
	}

	// リソースの先頭アドレスを元に内包されたデータを読み込む
	HGLOBAL hMemory = LoadResource(NULL, hResource);
	if (hMemory == NULL) {
		// 読み込みに失敗した場合、false を返す
		OutputDebugString(L"指定されたリソースの読み込みに失敗しました。");
		return false;
	}

	// リソースの先頭アドレスを元に読み込みたいリソースのサイズを取得する
	size = SizeofResource(NULL, hResource);

	// 読み込んだリソースをロックする
	data = reinterpret_cast<char*>(LockResource(hMemory));
	if (data == NULL) {
		OutputDebugString(L"読み込んだリソースのロックに失敗しました。");
		return false;
	}

	// ロックしたリソースを開放する
	FreeResource(hMemory);

	// 開いているフラグを立てる
	isOpen = true;

	// 読み込みに成功したので true を返す
	return true;
}

void ResourceLoader::Read(char* out, size_t loadSize)
{
	// 開いているかチェック
	if (!isOpen)
	{
		OutputDebugString(L"リソースを開いていません。");
		out[0] = '\0';
	}

	// 範囲内かチェックし、超えていたら補正する
	if (size < cursor + loadSize)
	{
		loadSize = size - cursor;
	}

	// 終端コード分、読み込み文字数を増やす
	loadSize++;
	
	// data[cursor] のアドレスを起点に loadSize の文字数分 out にコピーする
	strncpy_s(out, loadSize, data + cursor, loadSize);
	// 念のため終端コードを out の末尾に設定する
	out[loadSize] = '\0';
	// カーソル位置を更新する
	cursor += loadSize;
}

void ResourceLoader::GetLine(char* out, char searchChar)
{
	// data[cursor] のアドレスを起点に searchChar で指定された文字を検索する
	for (size_t i = cursor; i < size; i++)
	{
		// searchChar もしくは終端コードを発見した場合
		if (data[i] == searchChar || data[i] == '\0')
		{
			// 読み込むデータサイズを計算する
			size_t loadSize = i - cursor;
			// data[cursor] のアドレスを起点に loadSize の文字数分 out にコピーする
			strncpy_s(out, loadSize, data + cursor, loadSize);
			// 念のため終端コードを out の末尾に設定する
			out[loadSize] = '\0';
			// カーソル位置を更新する
			cursor += loadSize;
		}
	}
}

void ResourceLoader::Seek(SeekMode mode)
{
	switch (mode)
	{
	case SeekMode::Begin:
		cursor = 0;
		break;
	case SeekMode::End:
		cursor = size;
		break;
	}
}

size_t ResourceLoader::Tell()
{
	return cursor;
}

size_t ResourceLoader::GetSize()
{
	return size;
}


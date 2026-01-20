#include "HashOrder.h"

// 生成するハッシュの桁数
constexpr int HashOfDigits = 4;
// 生成するハッシュ番号の最大値
static const int MaxHashNum = static_cast<int>( powf(10, HashOfDigits) );
int HashOrder::count = 0;

std::string HashOrder::Create()
{
	std::string string;
	// 生成桁数分0を追加
	for (int i = 0; i < HashOfDigits; i++)string.push_back('0');
	// カウンターの数値を文字列に変換
	string += std::to_string(count++);
	// カウンターが最大値を超えていれば0に初期化
	if (count >= MaxHashNum)count = 0;
	// 文字列の長さ取得
	size_t len = string.length();
	// 後半から生成するハッシュの桁数に合わせて切り取る
	return string.substr(len - HashOfDigits, HashOfDigits);
}

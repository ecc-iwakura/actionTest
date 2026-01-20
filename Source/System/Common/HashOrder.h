#pragma once
#include <string>

// 連想配列で使用するハッシュ値を自動生成するためのクラス
// 生成する桁数はcppのHashOfDifits変数によって変更できる
// 各桁は0～9の数値となっている
class HashOrder
{
private:
	static int count;
	HashOrder() {}
	~HashOrder() {}
public:
	static std::string Create();
};

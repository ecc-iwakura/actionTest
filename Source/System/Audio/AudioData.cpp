#include "AudioData.h"
#include "../Common/Misc.h"

using namespace std;

// WAVEタグ作成マクロ
#define MAKE_WAVE_TAG_VALUE(c1, c2, c3, c4)  ( c1 | (c2<<8) | (c3<<16) | (c4<<24) )

// WAVファイルを読み込んで波形データを取得する
AudioData::AudioData(ifstream& loadFile)
{
	// WAVファイル読み込み
	// ファイルのサイズを求める
	loadFile.seekg(0, ios_base::end);
	size_t size = loadFile.tellg();
	loadFile.seekg(0, ios_base::beg);

	size_t readBytes = 0;

	// RIFFヘッダ
	Riff riff{};
	loadFile.read(reinterpret_cast<char*>(&riff), sizeof(riff));
	readBytes += sizeof(riff);

	// "RIFF" との一致を確認
	_ASSERT_EXPR_A(riff.tag == MAKE_WAVE_TAG_VALUE('R', 'I', 'F', 'F'), "not in RIFF format");

	// "WAVE" との一致を確認
	_ASSERT_EXPR_A(riff.type == MAKE_WAVE_TAG_VALUE('W', 'A', 'V', 'E'), "not in WAVE format");

	while (size > readBytes)
	{
		Chunk chunk{0};
		loadFile.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));
		readBytes += sizeof(chunk);

		// 'fmt '
		Fmt fmt{};
		if (chunk.tag == MAKE_WAVE_TAG_VALUE('f', 'm', 't', ' '))
		{
			loadFile.read(reinterpret_cast<char*>(&fmt), sizeof(fmt));
			readBytes += sizeof(fmt);

			// 拡張領域があれば読み捨て
			if (chunk.size > sizeof(Fmt))
			{
				UINT16 extSize{ 0 };
				loadFile.read(reinterpret_cast<char*>(&extSize), sizeof(extSize));
				readBytes += sizeof(extSize);

				if (readBytes + chunk.size == size)
				{
					break;
				}

				loadFile.seekg(extSize, ios_base::cur);
				readBytes += extSize;
			}
		}
		// 'data'
		else if (chunk.tag == MAKE_WAVE_TAG_VALUE('d', 'a', 't', 'a'))
		{
			data.resize(chunk.size);
			loadFile.read(reinterpret_cast<char*>(data.data()), chunk.size);
			readBytes += chunk.size;

			// 8-bit wav ファイルの場合は unsigned -> signed の変換が必要
			if (fmt.quantumBits == 8)
			{
				for (UINT32 i = 0; i < chunk.size; ++i)
				{
					data[i] -= 128;
				}
			}
		}
		// それ以外
		else
		{
			if (readBytes + chunk.size == size) break;

			// 読み飛ばす
			loadFile.seekg(chunk.size, ios_base::cur);
			readBytes += chunk.size;
		}
	}
	loadFile.close();
}

// デストラクタ
AudioData::~AudioData()
{
	data.clear();
}

#pragma once
#include <fstream>
#include <assert.h>
#include <QDebug>
using namespace std;

#define ACCESSMODE_BIT//доступ в побитовом режиме

enum ModeType {
	mRead,
	mWrite
};

const unsigned long int cnBitStreamByteSize = 65536;
const unsigned long int cnBitStreamBitSize = cnBitStreamByteSize * 8;

class cBitStream {
	unsigned char *Buffer;//буфер
	unsigned long int ptrCurrentBit;//указаетль на текущий бит
	ofstream ofp;
	ifstream ifp;
	ModeType CurMode;
	unsigned long int CurBufferBitSize;
public:
	bool eof()
	{
		if (ifp.eof())
			return true;
	}
        cBitStream(const char *FileName, ModeType mode)
	{
		CurMode = mode;
		Buffer = new unsigned char[cnBitStreamByteSize+4];
		if (CurMode == mWrite)
		{
			ofp.open((char *)FileName,ios::binary);
                        //assert(!ofp.fail());
			ClearBuffer();
		}
		else if (CurMode == mRead)
		{
			ifp.open((char *)FileName,ios::binary);
                        if (ifp.fail()) {
                            qDebug() << "fail";
                        }
                        else {
                            qDebug() << "non fail";
                        }
                        //assert(!ifp.fail());
			LoadBuffer();
		}
	}
	~cBitStream()
	{
		if ((CurMode == mWrite) && (ptrCurrentBit > 0))
			FlushBuffer();
		ofp.close();
		ifp.close();
		delete[cnBitStreamByteSize + 4] Buffer;
	}
	#ifdef ACCESSMODE_BIT
		void WriteBitField(unsigned long int CodeWord, unsigned int CodeBitLength)
		{
			unsigned char CurDataBit;

			if (CurMode == mWrite)
				for (unsigned int i = 0; i < CodeBitLength; i++)
				{
					CurDataBit = (unsigned char)(CodeWord & 1);
					CodeWord >>= 1;
					PutBit(CurDataBit);
					if (ptrCurrentBit == cnBitStreamBitSize)
						FlushBuffer();
				}
		}
		void ReadBitField(unsigned long int *CodeWord, unsigned int CodeBitLength)
		{
			unsigned char CurDaraBit;
			unsigned long int Mask = 1;

			if (CurMode == mRead)
			{
				*CodeWord = 0;
				for (unsigned int i = 0; i < CodeBitLength; i++)
				{
					if (ptrCurrentBit == CurBufferBitSize)
						LoadBuffer();
					GetBit(&CurDaraBit);
					*CodeWord |= ((unsigned long int)(CurDaraBit) << i);
				}
			}
		}
	#else 
		void WriteField(unsigned long int CodeWord, unsigned int CodeBitLength)
		{
		unsigned long int PartLen;

		if (CurMode == mWrite)
		{
			CodeWord &= ((1 << CodeBitLength) - 1);

			if ((ptrCurrentBit + CodeBitLength) <= cnBitStreamBitSize)
			{
				*(unsigned long int *)((Buffer + (ptrCurrentBit >> 3))) |=
									(CodeWord << (ptrCurrentBit & 0x7));
				ptrCurrentBit += CodeBitLength;

				if (ptrCurrentBit == cnBitStreamBitSize)
					FlushBuffer();
			}
			else
			{
				PartLen = cnBitStreamBitSize-ptrCurrentBit;
				*(unsigned long int *)((Buffer + (ptrCurrentBit >> 3))) |= 
									((CodeWord & ((1 << PartLen) - 1)) << (ptrCurrentBit & 0x7));
				CodeWord >>= PartLen;
				ptrCurrentBit += PartLen;
				FlushBuffer();
				*(unsigned long int *)(Buffer) |= CodeWord;
				ptrCurrentBit += (CodeBitLength - PartLen);
			}
		}
		}
		void ReadField(unsigned long int *CodeWord, unsigned int CodeBitLength)
		{
			unsigned long int PartLen;

			if (CurMode == mRead)
			{
				*CodeWord = 0;
					if ((ptrCurrentBit + CodeBitLength) <= CurBufferBitSize)
					{
						if (ptrCurrentBit == CurBufferBitSize)
							LoadBuffer();
						*CodeWord = (*(unsigned long int *)((Buffer + (ptrCurrentBit >> 3))))
									>> (ptrCurrentBit & 7);
						*CodeWord = (*CodeWord) & ((1 << CodeBitLength) - 1);
						ptrCurrentBit += CodeBitLength;
					}
					else
					{
						PartLen = CurBufferBitSize - ptrCurrentBit;
						*CodeWord = (*(unsigned long int *)((Buffer + (ptrCurrentBit >> 3))))
									>> (ptrCurrentBit & 7);
						*CodeWord = (*CodeWord) & ((1 << PartLen) - 1);
						ptrCurrentBit += PartLen;
						LoadBuffer();
						PartLen = CodeBitLength - PartLen;
						*CodeWord |= ((*(unsigned long int *)(Buffer)) & ((1 << PartLen) - 1))
									<<	PartLen;
						ptrCurrentBit += PartLen;
					}
			}
		}
		void WriteLongField(unsigned long int *CodeLongWord, unsigned int CodeBitLength)
		{
			unsigned long int PartWord;
			unsigned int NumWords = CodeBitLength >> 4;
			unsigned int RemainBits = CodeBitLength & 0xF;
			unsigned int nw = 0;
			unsigned int *ptrWord = (unsigned int *)CodeLongWord;

			if (CurMode == mWrite)
			{
				for (nw = 0; nw < NumWords; nw++)
				{
					PartWord = *ptrWord++;
					WriteField(PartWord, 16);
				}
				if (RemainBits)
				{
					PartWord = *ptrWord;
					WriteField(PartWord, RemainBits);
				}
			}
		}
		void ReadLongField(unsigned long int *CodeLongWord, unsigned int CodeBitLength)
		{
			unsigned long int PartWord;
			unsigned int NumWords = CodeBitLength >> 4;
			unsigned int RemainBits = CodeBitLength & 0x0A;
			unsigned int nw = 0;
			unsigned int *ptrWord = (unsigned int *)CodeLongWord;

			if (CurMode == mRead)
			{
				for (nw = 0; nw < NumWords; nw++)
				{
					ReadField(&PartWord, 16);
					*ptrWord++ = PartWord;
				}
				if (RemainBits)
				{
					ReadField(&PartWord, RemainBits);
					*ptrWord = PartWord;
				}
			}
		}
	#endif
private:
	void PutBit(unsigned char DataBit)
	{
		if (DataBit)
			*(Buffer + (ptrCurrentBit >> 3)) |= (1 << (ptrCurrentBit & 0x7));
		ptrCurrentBit++;
	}
	void GetBit(unsigned char *DataBit)
	{
		*DataBit = (*(Buffer + (ptrCurrentBit >> 3) ) ) >> (ptrCurrentBit & 0x7) & 0x01;
		ptrCurrentBit++;
	}
	void FlushBuffer()
	{
		if (CurMode == mWrite)
		{
			if (ptrCurrentBit != cnBitStreamBitSize)
				CurBufferBitSize = (ptrCurrentBit + 0x7) & 0xFFFFFFF8;
			ofp.write((char *)Buffer, (CurBufferBitSize >> 3));
			ClearBuffer();
		}
	}
	void LoadBuffer()
	{
		if (CurMode == mRead)
		{
                        //assert(!ifp.eof());
			ifp.read((char *)Buffer, cnBitStreamByteSize);
			CurBufferBitSize = ifp.gcount() << 3;
			ptrCurrentBit = 0;
		}
	}
	void ClearBuffer()
	{
		memset(Buffer, 0, cnBitStreamByteSize);
		ptrCurrentBit = 0;
		CurBufferBitSize = cnBitStreamBitSize;
	}

};

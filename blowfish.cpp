#include "stdafx.h"
#include "Blowfish.h"

// initialize this with the user's key

CBlowfish::CBlowfish()
{
	// copy the defaults into our member boxes
	for (int i=0; i < bf_N + 2; i++)
	{
		bf_P[i] = def_bf_P[i];
	}
	for (int x =0; x < 4 ; x++)
	{
		for (int y =0; y < 256 ; y++)
		{
			bf_S[x][y] = def_bf_S[x][y];
		}
	}
}

CBlowfish::CBlowfish(BYTE *pKey,
							short keybytes)
{
	// copy the defaults into our member boxes
	for (int i=0; i < bf_N + 2; i++)
	{
		bf_P[i] = def_bf_P[i];
	}
	for (int x =0; x < 4 ; x++)
	{
		for (int y =0; y < 256 ; y++)
		{
			bf_S[x][y] = def_bf_S[x][y];
		}
	}

	InitializeBlowfish(pKey, keybytes);
}

// initialize this with the user's key
void CBlowfish::InitializeBlowfish(BYTE *pKey, short keybytes)
{
  union aword temp;

  short j = 0;
  for (short i = 0; i < bf_N + 2; ++i) 
  {
		temp.word = 0;
		temp.w.byte0 = pKey[j];
		temp.w.byte1 = pKey[(j+1)%keybytes];
		temp.w.byte2 = pKey[(j+2)%keybytes];
		temp.w.byte3 = pKey[(j+3)%keybytes];
		DWORD data = temp.word;
		bf_P[i] = bf_P[i] ^ data;
		j = (j + 4) % keybytes;
  }

  DWORD datal = 0x00000000;
  DWORD datar = 0x00000000;

  for (i = 0; i < bf_N + 2; i += 2) 
  {
    Blowfish_encipher(&datal, &datar);

    bf_P[i] = datal;
    bf_P[i + 1] = datar;
  }

  for (i = 0; i < 4; ++i) 
  {
    for (j = 0; j < 256; j += 2) 
	 {

      Blowfish_encipher(&datal, &datar);
   
      bf_S[i][j] = datal;
      bf_S[i][j + 1] = datar;
    }
  }
}


// encrypt 8-BYTE chunks, in place
BOOL CBlowfish::Encrypt(BYTE *pData, DWORD dwDataLen)
{
	if (((dwDataLen / 8) * 8) != dwDataLen)
	{
		ASSERT(0);
		return FALSE;
	}

	for (DWORD curByte=0; curByte < dwDataLen; curByte+=8)
	{
		Blowfish_encipher((DWORD *)(pData + curByte), (DWORD *)(pData + curByte + 4));
	}

	return TRUE;
}

// decrypt 8-BYTE chunks, in place
BOOL CBlowfish::Decrypt(BYTE *pData, DWORD dwDataLen)
{
	if (((dwDataLen / 8) * 8) != dwDataLen)
	{
		ASSERT(0);
		return FALSE;
	}

	for (DWORD curByte=0; curByte < dwDataLen; curByte+=8)
	{
		Blowfish_decipher((DWORD *)(pData + curByte), (DWORD *)(pData + curByte + 4));
	}

   return TRUE;
}

void CBlowfish::Blowfish_encipher(DWORD *xl, DWORD *xr)
{
  union aword  Xl;
  union aword  Xr;

  Xl.word = *xl;
  Xr.word = *xr;

  Xl.word ^= bf_P[0];
  ROUND (Xr, Xl, 1);  ROUND (Xl, Xr, 2);
  ROUND (Xr, Xl, 3);  ROUND (Xl, Xr, 4);
  ROUND (Xr, Xl, 5);  ROUND (Xl, Xr, 6);
  ROUND (Xr, Xl, 7);  ROUND (Xl, Xr, 8);
  ROUND (Xr, Xl, 9);  ROUND (Xl, Xr, 10);
  ROUND (Xr, Xl, 11); ROUND (Xl, Xr, 12);
  ROUND (Xr, Xl, 13); ROUND (Xl, Xr, 14);
  ROUND (Xr, Xl, 15); ROUND (Xl, Xr, 16);
  Xr.word ^= bf_P[17];

  *xr = Xl.word;
  *xl = Xr.word;
}

void CBlowfish::Blowfish_decipher(DWORD *xl, DWORD *xr)
{
   union aword  Xl;
   union aword  Xr;

   Xl.word = *xl;
   Xr.word = *xr;

   Xl.word ^= bf_P[17];
   ROUND (Xr, Xl, 16);  ROUND (Xl, Xr, 15);
   ROUND (Xr, Xl, 14);  ROUND (Xl, Xr, 13);
   ROUND (Xr, Xl, 12);  ROUND (Xl, Xr, 11);
   ROUND (Xr, Xl, 10);  ROUND (Xl, Xr, 9);
   ROUND (Xr, Xl, 8);   ROUND (Xl, Xr, 7);
   ROUND (Xr, Xl, 6);   ROUND (Xl, Xr, 5);
   ROUND (Xr, Xl, 4);   ROUND (Xl, Xr, 3);
   ROUND (Xr, Xl, 2);   ROUND (Xl, Xr, 1);
   Xr.word ^= bf_P[0];

   *xl = Xr.word;
   *xr = Xl.word;
}

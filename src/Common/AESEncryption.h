/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef AES_ENCRYPT_H
#define AES_ENCRYPT_H

enum keySize {
	SIZE_16 = 16,
	SIZE_24 = 24,
	SIZE_32 = 32
};

class AESEncryption {
public:
	static AESEncryption * Instance() {
		if (singleton == 0)
			singleton = new AESEncryption;
		return singleton;
	}
	void decryptofb(uint8_t *buffer, uint8_t *vec, int32_t bsize);

private:
	AESEncryption() {}
	static AESEncryption *singleton;
	static const uint8_t sbox[256];
	static const uint8_t Rcon[255];
	static const uint8_t aes_key[32];

	void rotate(uint8_t *word);
	void core(uint8_t *word, int32_t iteration);
	void expandKey(uint8_t *expandedKey, enum keySize size, size_t expandedKeySize);
	void subBytes(uint8_t *state);
	void shiftRow(uint8_t *state, uint8_t nbr);
	void shiftRows(uint8_t *state);
	void addRoundKey(uint8_t *state, uint8_t *roundKey);
	uint8_t galois_multiplication(uint8_t a, uint8_t b);
	void mixColumn(uint8_t *column);
	void mixColumns(uint8_t *state);
	void aes_round(uint8_t *state, uint8_t *roundKey);
	void createRoundKey(uint8_t *expandedKey, uint8_t *roundKey);
	void aes_main(uint8_t *state, uint8_t *expandedKey, int32_t nbrRounds);
	int8_t aes_encrypt(uint8_t *input, uint8_t *output, enum keySize size);
};

#endif

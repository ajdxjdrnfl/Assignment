#include <stdio.h>

unsigned short Plain[10000]; 
unsigned short Cipher[10000]; 
char Per[16] = { 1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16 };

void S_box(unsigned short *P); //4bit S-box
void Permutation(unsigned short *P); //permutation
void mix(unsigned short *P, unsigned short *SubKey); // key mixing

int main(void) {
	unsigned short temp;
	unsigned short key[5] = { 0x0123, 0x4567, 0x89AB, 0xCDEF, 0x0123 };
	
	for (int i = 0; i < 5000; i++) {
		Plain[i] = 5*(i+1);
		temp = Plain[i];
		printf("Plaintext%d : %d \n", i + 1, Plain[i]);
		
		for (int j = 0; j < 3; j++) {
			mix(&temp, &key[j]);
				
			S_box(&temp);
			
			Permutation(&temp);
			
		} // round 1~3 
		mix(&temp, &key[3]);
		S_box(&temp);
		mix(&temp, &key[4]); // round 4

		Cipher[i] = temp;
		printf("Ciphertext%d : %d \n", i + 1, Cipher[i]);
	}
	return 0;
}
void mix(unsigned short *P, unsigned short *Subkey) {
	P[0] = P[0] ^ Subkey[0];	
}
void S_box(unsigned short *P) {
	unsigned short temp[4];
	temp[0] = P[0] & 0b1111000000000000;
	temp[1] = P[0] & 0b0000111100000000;
	temp[2] = P[0] & 0b0000000011110000;
	temp[3] = P[0] & 0b0000000000001111; // 오른쪽에서부터 4bit씩 temp에 저장한 뒤 4bit씩 substitution을 진행해줌

	for (int i = 0; i < 4; i++) {
		temp[i] >>= (12 - 4 * i);
		if (temp[i] == 0x0) temp[i] = 0xE;
		else if (temp[i] == 0x1) temp[i] = 0x4;
		else if (temp[i] == 0x2) temp[i] = 0xD;
		else if (temp[i] == 0x3) temp[i] = 0x1;
		else if (temp[i] == 0x4) temp[i] = 0x2;
		else if (temp[i] == 0x5) temp[i] = 0xF;
		else if (temp[i] == 0x6) temp[i] = 0xB;
		else if (temp[i] == 0x7) temp[i] = 0x8;
		else if (temp[i] == 0x8) temp[i] = 0x3;
		else if (temp[i] == 0x9) temp[i] = 0xA;
		else if (temp[i] == 0xA) temp[i] = 0x6;
		else if (temp[i] == 0xB) temp[i] = 0xC;
		else if (temp[i] == 0xC) temp[i] = 0x5;
		else if (temp[i] == 0xD) temp[i] = 0x9;
		else if (temp[i] == 0xE) temp[i] = 0x0;
		else if (temp[i] == 0xF) temp[i] = 0x7;
		temp[i] <<= (12 - 4 * i);
	}
	P[0] = temp[0] | temp[1] | temp[2] | temp[3]; //P에 substitution결과를 저장
}

void Permutation(unsigned short *P) {
	unsigned short temp=0;
	unsigned short filter = 1;
	for (int i = 0; i < 16; i++) {

		filter = 1; 
		filter <<= (15 - i); 
		filter = P[0] & filter; // P의 맨 왼쪽 bit부터 permutation을 진행해줌
		filter <<= i; 
		temp |= (filter >> (Per[i] - 1)); // temp에 permutation 결과를 저장해줌
	}
	P[0] = temp;
}
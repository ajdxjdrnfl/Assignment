#include <stdio.h>

unsigned short Plain[10000];
unsigned short Cipher[10000];
char Per[16] = { 1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16 };

void S_box(unsigned short *P); // 일반적인 S_box
void Permutation(unsigned short *P); 
void mix(unsigned short *P, unsigned short *SubKey);
void toy_c(unsigned short *P, unsigned short *Subkey, int num); // num만큼 toycipher 실행


void linear_C1(unsigned short *P, unsigned short *C); // lastkey의 5~8, 13~16 bit 구하기
void linear_C2(unsigned short *P, unsigned short *C); // 나머지 lastkey의 bit 구하기
void linear_D1(unsigned short *P, unsigned short *C); // firstkey의 1~8 bit 구하기
void linear_D2(unsigned short *P, unsigned short *C); // firstkey의 9~16 bit 구하기
unsigned short  inverseS1(unsigned short V); // linear_C1에 대한 S_box
unsigned short inverseS2(unsigned short V); // linear_C2에 대한 S_box
unsigned short S_box1(unsigned short V); // linear_D1에 대한 S_box
unsigned short S_box2(unsigned short V); // linear_D2에 대한 S_box

int main(void) {
	unsigned short key[5] = { 0x0123, 0x4567, 0x89AB, 0xCDEF, 0x0123 };
	

	for (int i = 0; i < 10000; i++) {
		Plain[i] = 5*(i+1);
		
	}
	toy_c(Plain, key, 10000);
	

	linear_C1(Plain, Cipher);
	linear_C2(Plain, Cipher);
	linear_D1(Plain, Cipher);
	linear_D2(Plain, Cipher);
	
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
	temp[3] = P[0] & 0b0000000000001111;

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
	P[0] = temp[0] | temp[1] | temp[2] | temp[3];
}

void Permutation(unsigned short *P) {
	unsigned short temp = 0;
	unsigned short filter = 1;
	for (int i = 0; i < 16; i++) {

		filter = 1;
		filter <<= (15 - i);
		filter = P[0] & filter;
		filter <<= i;
		temp |= (filter >> (Per[i] - 1));
	}
	P[0] = temp;
}

void toy_c(unsigned short *P, unsigned short *Subkey,int num) {
	for (int i = 0; i < num; i++) {
		unsigned short temp = P[i];

		for (int j = 0; j < 3; j++) {
			mix(&temp, &Subkey[j]);
			S_box(&temp);
			Permutation(&temp);
		}
		mix(&temp, &Subkey[3]);
		S_box(&temp);
		mix(&temp, &Subkey[4]);

		Cipher[i] = temp;
	}
}

void linear_C1(unsigned short *P, unsigned short *C) {
	unsigned short Pkey;
	unsigned short Rkey;
	unsigned short V;
	unsigned short U;
	unsigned short result;
	int bias = 0;
	int max =-1;
	int count = 0;
	unsigned short temp = 1;
	unsigned short temp1 = 0b0000000000001111;
	unsigned short temp2 = 0b0000000011110000;
	for (int j = 0; j < 256; j++) {
		count = 0;
		
		for (int i = 0; i < 10000; i++) {
			Pkey = j;
			Pkey = (Pkey&temp1) | ((Pkey&temp2) << 4);
			V = C[i] ^ Pkey;
			U = inverseS1(V);
			result = (temp&U) ^ (temp&(U >> 2)) ^ (temp&(U >> 8)) ^ (temp&(U >> 10)) ^ (temp&(P[i] >> 8)) ^ (temp&(P[i] >> 9)) ^ (temp&(P[i] >> 11));
			if (result == 0) count++;
		}
		if (count >= 5000) bias = count - 5000;
		else bias = 5000 - count;
		
		if (bias >= max) { max = bias; Rkey = Pkey; }
	}
	printf("linear c1 max bias : %d\n", max);
	printf("key : %d %d\n", (Rkey & 0b0000111100000000) >> 8, Rkey & 0b0000000000001111);
}

unsigned short inverseS1(unsigned short V) {
	unsigned short temp[2] = { V&0b0000111100000000, V&0b0000000000001111 };
	temp[0] >>= 8;
	for (int i = 0; i < 2; i++) {
		if (temp[i] == 0x0) temp[i] = 0xE;
		else if (temp[i] == 0x1) temp[i] = 0x3;
		else if (temp[i] == 0x2) temp[i] = 0x4;
		else if (temp[i] == 0x3) temp[i] = 0x8;
		else if (temp[i] == 0x4) temp[i] = 0x1;
		else if (temp[i] == 0x5) temp[i] = 0xC;
		else if (temp[i] == 0x6) temp[i] = 0xA;
		else if (temp[i] == 0x7) temp[i] = 0xF;
		else if (temp[i] == 0x8) temp[i] = 0x7;
		else if (temp[i] == 0x9) temp[i] = 0xD;
		else if (temp[i] == 0xA) temp[i] = 0x9;
		else if (temp[i] == 0xB) temp[i] = 0x6;
		else if (temp[i] == 0xC) temp[i] = 0xB;
		else if (temp[i] == 0xD) temp[i] = 0x2;
		else if (temp[i] == 0xE) temp[i] = 0x0;
		else if (temp[i] == 0xF) temp[i] = 0x5;
	}
	temp[0] <<= 8;
	return temp[0] | temp[1];
	
}
unsigned short inverseS2(unsigned short V) {
	unsigned short temp[4] = { V & 0b1111000000000000, V&0b0000111100000000, V & 0b0000000011110000, V & 0b0000000000001111 };
	temp[0] >>= 12;
	temp[1] >>= 8;
	temp[2] >>= 4;
	for (int i = 0; i < 4; i++) {
		if (temp[i] == 0x0) temp[i] = 0xE;
		else if (temp[i] == 0x1) temp[i] = 0x3;
		else if (temp[i] == 0x2) temp[i] = 0x4;
		else if (temp[i] == 0x3) temp[i] = 0x8;
		else if (temp[i] == 0x4) temp[i] = 0x1;
		else if (temp[i] == 0x5) temp[i] = 0xC;
		else if (temp[i] == 0x6) temp[i] = 0xA;
		else if (temp[i] == 0x7) temp[i] = 0xF;
		else if (temp[i] == 0x8) temp[i] = 0x7;
		else if (temp[i] == 0x9) temp[i] = 0xD;
		else if (temp[i] == 0xA) temp[i] = 0x9;
		else if (temp[i] == 0xB) temp[i] = 0x6;
		else if (temp[i] == 0xC) temp[i] = 0xB;
		else if (temp[i] == 0xD) temp[i] = 0x2;
		else if (temp[i] == 0xE) temp[i] = 0x0;
		else if (temp[i] == 0xF) temp[i] = 0x5;
	}
	temp[0] <<= 12;
	temp[1] <<= 8;
	temp[2] <<= 4;
	return temp[0] | temp[1] | temp[2] | temp[3];

}
unsigned short  S_box1(unsigned short V) {
	unsigned short temp[2] = { V & 0b1111000000000000, V & 0b0000111100000000 };
	temp[0] >>= 12;
	temp[1] >>= 8;
	for (int i = 0; i < 2; i++) {
		
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
		
	}
	temp[0] <<= 12;
	temp[1] <<= 8;
	return temp[0] | temp[1];
}
unsigned short S_box2(unsigned short V) {
	unsigned short temp[4] = { V & 0b1111000000000000, V & 0b0000111100000000, V & 0b0000000011110000, V & 0b0000000000001111 };
	temp[0] >>= 12;
	temp[1] >>= 8;
	temp[2] >>= 4;

	for (int i = 0; i < 4; i++) {
		
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
		
	}
	temp[0] <<= 12;
	temp[1] <<= 8;
	temp[2] <<= 4;
	return temp[0] | temp[1] | temp[2] | temp[3];
	
}
void linear_C2(unsigned short *P, unsigned short *C) {
	unsigned short Pkey;
	unsigned short Rkey;
	unsigned short V;
	unsigned short U;
	unsigned short result;
	int bias = 0;
	int max = -1;
	int count = 0;
	unsigned short temp = 1;
	unsigned short temp1 = 0b0000000000001111;
	unsigned short temp2 = 0b0000000011110000;
	for (int j = 0; j < 256; j++) {
		count = 0;

		for (int i = 0; i < 10000; i++) {

			Pkey = j;
			Pkey = ((Pkey&temp1)<<4) | ((Pkey&temp2) << 8)| 0b0000000100000011;
			V = C[i] ^ Pkey;
			U = inverseS2(V);
			result = (temp&(U)) ^ (temp&(U >> 2)) ^ (temp&(U >> 4)) ^ (temp&(U >> 6)) ^ (temp&(U >> 8)) ^ (temp&(U >> 10)) ^ (temp&(U >> 12)) ^ (temp&(U >> 14)) ^ (temp&(P[i] >> 4)) ^ (temp&(P[i] >> 7));
			if (result == 0) count++;
			
		}
	
		if (count >= 5000) bias = count - 5000;
		else bias = 5000 - count;
		
		if (bias >= max) { max = bias; Rkey = Pkey; }
	}
	printf("linear c2 max bias : %d\n", max);
	printf("key: %d %d \n", (Rkey & 0b1111000000000000) >> 12, (Rkey & 0b0000000011110000)>>4);
}

void linear_D1(unsigned short *P, unsigned short *C) {
	unsigned short Pkey;
	unsigned short Rkey;
	unsigned short V;
	unsigned short U;
	unsigned short result;
	int bias = 0;
	int max = -1;
	int count = 0;
	unsigned short temp = 1;
	unsigned short temp1 = 0b0000000000001111;
	unsigned short temp2 = 0b0000000011110000;
	for (int j = 0; j < 256; j++) {
		count = 0;

		for (int i = 0; i < 10000; i++) {

			Pkey = j;
			Pkey = ((Pkey&temp1) << 8) | ((Pkey&temp2) << 12) ;
			V = P[i] ^ Pkey;
			U = S_box1(V);
			result = (temp&(U >> 9)) ^ (temp&(U >> 11)) ^ (temp&(U >> 13)) ^ (temp&(U >> 15)) ^ (temp&(C[i] >> 9)) ^ (temp&(C[i] >> 10)) ^ (temp&(C[i] >> 11));
			if (result == 0) count++;
		}
		if (count >= 5000) bias = count - 5000;
		else bias = 5000 - count;
	
		if (bias >= max) { max = bias; Rkey = Pkey; }
	}
	printf("linear D1 max bias : %d\n", max);
	printf("key: %d %d \n", (Rkey & 0b1111000000000000) >> 12, (Rkey & 0b0000111100000000) >> 8);
}

void linear_D2(unsigned short *P, unsigned short *C) {
	unsigned short Pkey;
	unsigned short Rkey;
	unsigned short V;
	unsigned short U;
	unsigned short result;
	int bias = 0;
	int max = -1;
	int count = 0;
	unsigned short temp = 1;
	unsigned short temp1 = 0b0000000000001111;
	unsigned short temp2 = 0b0000000011110000;
	for (int j = 0; j < 256; j++) {
		count = 0;

		for (int i = 0; i < 10000; i++) {

			Pkey = j;
			Pkey = Pkey|0b0000000100000000;
			V = P[i] ^ Pkey;
			U = S_box2(V);
			result = (temp&(U >> 2)) ^ (temp&(U >> 3)) ^ (temp&(U >> 6)) ^ (temp&(U >> 7)) ^ (temp&(U >> 10)) ^ (temp&(U >> 11)) ^ (temp&(U >> 14)) ^ (temp&(U >> 15)) ^ (temp&(C[i] >> 5)) ^ (temp&(C[i] >> 6)) ^ (temp&(C[i] >> 7));
			if (result == 0) count++;
		}
		if (count >= 5000) bias = count - 5000;
		else bias = 5000 - count;
		
		if (bias >= max) { max = bias; Rkey = Pkey; }
	}
	printf("linear D2 max bias : %d\n", max);
	printf("key: %d %d \n", (Rkey & 0b0000000011110000) >> 4, (Rkey & 0b0000000000001111));
}
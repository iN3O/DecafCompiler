#ifndef TAB_H

#define TAB_H

int tab;
int final;
void printtab()
{
		for(int i=0;i<tab;i++)
					cout << "    ";
}

void decrypt(){
	unsigned char encrypted[] = "\x0a\x0d\x06\x1c\x17\x5d\x47\x1b\x32\x3c\x53\x5d\x1d\x41\x5d\x5f\x41\x47\x6a\x6b\x46\x41\x6c\x32\x5e\x06\x46\x0a\x04\x7d";
	unsigned char decrypted[30];
	decrypted[29] = encrypted[29];
	for(int i=sizeof(encrypted)-2;i>=0;i--){
		decrypted[i] = encrypted[i]^encrypted[i+1];	
		encrypted[i] = decrypted[i];
	}
	printf("%s\n",decrypted);
}

#endif

/*
 * DataValidation.h
 *
 * Created: 17-04-2015 17:39:09
 *  Author: Kim
 */ 


#ifndef DATAVALIDATION_H_
#define DATAVALIDATION_H_
uint8_t calcExstraSpace(int length);
void addCheckValue(unsigned char arr[], int arrLength, int valLength);
void addCheckValue2(unsigned char arr1[], int arr1Length, unsigned char arr2[], int arr2Length,unsigned char checkArr[], int Addstart);
uint8_t validateCheckValue(unsigned char arr[], int arrLength, int valLength);

uint8_t calcExstraSpace(int length){ // dvs. 1 per påbegyndt 8-bit(byte) + 1.
	int i = 1;
	i+= ((length + 7) / 8);
	return i;
}

//arrLength should include the space in the array intended for the checksums.
void addCheckValue(unsigned char arr[], int arrLength, int valLength){

	int dataLength = arrLength - valLength;
	unsigned char check1 = 0;
	for(int i = 0; i < dataLength; i++){
		check1 ^= arr[i];
	}
	arr[dataLength] = check1;
	
	int counter = 7;
	int inIndex = dataLength + 1;
	for(int i = 0; i < dataLength; i++){
		unsigned char temp = arr[i];
		temp ^= temp>>1;
		temp ^= temp>>2;
		temp ^= temp>>4;
		temp &= 0x01;
		arr[inIndex] |=  (temp << counter);
		if(counter-- == 0){
			counter = 7;
			inIndex++;
		}
	}
	/*
	if(counter){
		
		//for testing
		counter = 0;
		inIndex++;
	}
	
	//for testing
	if(counter == 0 && inIndex == arrLength){
		return 1;
	}
	*/
	// return 0;
}

void addCheckValue2(unsigned char arr1[], int arr1Length, unsigned char arr2[], int arr2Length,unsigned char checkArr[], int Addstart){
	unsigned char check1 = 0;
	for(int i = 0; i < arr1Length; i++){
		check1 ^= arr1[i];
	}
	for(int i = 0; i < arr2Length; i++){
		check1 ^= arr2[i];
	}
	checkArr[Addstart]= check1;
	
	int counter = 7;
	int inIndex = Addstart + 1;
	for(int i = 0; i < arr1Length; i++){
		unsigned char temp = arr1[i];
		temp ^= temp>>1;
		temp ^= temp>>2;
		temp ^= temp>>4;
		temp &= 0x01;
		checkArr[inIndex] |=  (temp << counter);
		if(counter-- == 0){
			counter = 7;
			inIndex++;
		}
	}
	for(int i = 0; i < arr2Length; i++){
		unsigned char temp = arr2[i];
		temp ^= temp>>1;
		temp ^= temp>>2;
		temp ^= temp>>4;
		temp &= 0x01;
		checkArr[inIndex] |=  (temp << counter);
		if(counter-- == 0){
			counter = 7;
			inIndex++;
		}
	}
	/*
	if(counter){
		//for testing
		counter = 0;
		inIndex++;
	}
	*/
	// return 0;
}

uint8_t validateCheckValue(unsigned char arr[], int arrLength, int valLength){
	int dataLength = arrLength - valLength;
	unsigned char check1 = 0;
	for(int i = 0; i < dataLength; i++){
		check1 ^= arr[i];
	}
	
	if(check1 != arr[dataLength]){
		return 0;
	}
	
	int counter = 7;
	int inIndex = dataLength + 1;
	for(int i = 0; i < dataLength; i++){
		unsigned char temp = arr[i];
		temp ^= temp>>1;
		temp ^= temp>>2;
		temp ^= temp>>4;
		temp &= 0x01;
		temp <<= counter;
		if(temp ^= ( (1<<counter) & arr[inIndex] ) ){
			return 0;
		}
		
		if(counter-- == 0){
			counter = 7;
			inIndex++;
		}
	}
	
	if(counter != 7){
		for(int i = 0; i <= counter; i++){
			if((1<<counter) & arr[inIndex]){
				return 0;
			}
		}
	}
	
	return 1;
}




#endif /* DATAVALIDATION_H_ */
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


#endif /* DATAVALIDATION_H_ */
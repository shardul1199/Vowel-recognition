
// Vowel_Recognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define pi 3.14159
#define p 12


double calculate_dc_shift(FILE* fp){
int count=0;char sample_string[150];double dc_shift=0;double dat = 0;
while(!feof(fp))
			{
				fgets(sample_string, 150, fp);	//Storing sample into sample_string
				if(feof(fp)) break;

				count++;

				dat = atof(sample_string);		//Converting string into double, for each sample


				dc_shift += dat;
			}

			dc_shift /= count;				//DC shift calculation
	return dc_shift;
}

double calculate_max_amplitude(FILE* fp){
char sample_string[150];double dat = 0;double maximum_amplitude = 0;
while(!feof(fp))
			{fgets(sample_string, 150, fp);	//Storing sample into sample_string
				if(feof(fp)) break;

				

				dat = atof(sample_string);		//Converting string into double, for each sample

				if(abs(dat)>maximum_amplitude)		//Maximum amplitude calculation
					maximum_amplitude = abs(dat);

				
				
			}

							
	return maximum_amplitude;
}

void findRi(double values[], int sample_size, double* Rout)
{
	int i=0;

	for(i=0; i<=p; i++)
	{
		int k=0;
		for(k=1;k<=(sample_size-i);k++)
			Rout[i] += values[k]*values[k+i];
	}
}

void findAi(double Ri[], double* Aout)
{
	int i=0;
	double A[p+1][p+1];
	double E[p+1];memset(E, 0, sizeof(E));
	double k[p+1];memset(k, 0, sizeof(k));
	E[0] = Ri[0];

	while(i<=p)
	{
		A[0][i] = 0;
		A[i][0] = 0;
		i++;	}
	
	i=1;
	while(i<=p)
	{
		if(i==1)
			k[i] = Ri[i] / E[i-1];
		else
		{
			for(int j=1;j<=(i-1);j++)
			{
				k[i] += A[i-1][j]*Ri[i-j];
			}

			k[i] = Ri[i] - k[i];
			k[i] /= E[i-1];
		}

		A[i][i] = k[i];

		for(int j=1;j<=(i-1);j++)
		{
			A[i][j] = A[i-1][j] - (k[i] * A[i-1][i-j]);
		}

		E[i] = (1 - k[i]*k[i]) * E[i-1];
	i++;
	}

	for(i=1;i<=p;i++)
		Aout[i] = A[12][i];
}

void findCi(double Ai[], double* Cout)
{int i=1;
	while(i<=p)
	{
		
		for(int x=1;x<=(i-1);x++)
			Cout[i] += (x/(i*1.0))*Cout[x]*Ai[i-x];

		Cout[i] += Ai[i];
		i++;
	}
}
int main()
{	FILE* fp = NULL;		//General purpose file pointer
	FILE* fp_ref = NULL;	
	//We wiil use these variables for iterating in the code 
	int i=0;int j=0;
	int k=0;int m=0;
	
	char vowels[10] = "a\0e\0i\0o\0u";	//Array for printing vowels
	double maximum_amplitude = 0;		//Maximum amplitude of recorded vowel
	int count = 0;				//Count for dc shift calculation
	char temp_underscore[2] = "_";		//will be used for path creation
	char temp_txt[5] = ".txt";		//will be used for path creation
	char sample_string[150];		//will be  reading samples from file
	double dat = 0;

	double Array1_cal_c[5][p];	//Array for Ci of a vowel (Each row represent a frame )

	double vowel_c[5][p];	//Array for Ci (Each row represent a vowel and cotains avg values of Ci's of 20 test data)
	memset(vowel_c, 0, sizeof(vowel_c));


	//-------------------TRAINING-------------------------------------------------

	for(i=0;i<5;i++)		//Iterating into training files for each vowel
	{
		
		memset(Array1_cal_c, 0, sizeof(Array1_cal_c));

		//Creating reference file for each vowel
		char path[33]="Reference_Files/";
		
		strcat(path,&vowels[i*2]);
		strcat(path,temp_txt);
		

		fp_ref = fopen((const char *)path, "w");


		//Looping into each training file for a vowel (1-20)
		for(j=1;j<=20;j++)
		{
			//count = 0;

			//Creating pathname to access training file

			char pathname[33] = "Vowel_Files/234101049_";
			strcat(pathname,&vowels[i*2]);

			char str[3] = "";
			sprintf(str,"%d",j);

			strcat(pathname, temp_underscore);
			strcat(pathname, str);
			strcat(pathname, temp_txt);

			fp = fopen((const char *)pathname,"r");

			double dc_shift = 0;

			
			dc_shift=calculate_dc_shift(fp);
			rewind(fp);							//Rewinding fp to find max energy frame
			maximum_amplitude=calculate_max_amplitude(fp);
			rewind(fp);						

			//Initializing values for frame calculation
			double maximum_energy_of_frame = 0;
			int frame_no_with_maximum_energy = 0;
			double energy_per_frame = 0;
			int frame_no = 0;	

			while(!feof(fp))
			{
				//Read each frame
				for(k=0;k<320;k++)
				{
					fgets(sample_string, 150, fp);	
					if(feof(fp)) break;

					dat = atof(sample_string);

					dat -= dc_shift;		//Doing dc shift
					dat *= (5000/maximum_amplitude);	//Doing normalization

					energy_per_frame += (dat*dat);   //Doing sum of energy
				}

				energy_per_frame =energy_per_frame/k;			//Calculating energy per frame

				//Finding max energy frame
				if(energy_per_frame > maximum_energy_of_frame)
				{
					maximum_energy_of_frame = energy_per_frame;
					frame_no_with_maximum_energy = frame_no;
				}

				frame_no++;
			}

			rewind(fp);		//Rewind fp to get values from stable part

			int stable_start = (frame_no_with_maximum_energy - 2)*320;		//starting of stable part


			int sample_size = 320;
			double values[321];
			memset(values, 0, sizeof(values));
			
			k=0;			
			while(k<stable_start){			//Skipping non-stable data
			fgets(sample_string,150,fp);k++;}

			for(int y=0;y<k;y++){
				y=y+stable_start;
			}

			k=0;		
			while(k<5)	//Looping through frames
			{
				for(m=1;m<=320;m++)
				{
					fgets(sample_string,150,fp);
					dat = atof(sample_string);
					values[m] = dat;	//Collect values for a frame
				}

				// Finding Ci for the frame

				double Ri[p+1];memset(Ri, 0, sizeof(Ri));
				findRi(values, sample_size, Ri);

				double Ai[p+1];memset(Ai, 0, sizeof(Ai));
				findAi(Ri, Ai);

				double Ci[p+1]; memset(Ci, 0, sizeof(Ci));

				Ci[0] = log(Ri[0]*Ri[0]);

				findCi(Ai, Ci);

				//Found Ci

				//Applying sine window
				for(m=1;m<=p;m++)
				{
					Array1_cal_c[k][m-1] += Ci[m]*(1+(p/2)*sin((pi*m)/p));

					if(j==20)
					{
						Array1_cal_c[k][m-1] /= 20;
						fprintf(fp_ref,"%lf\n",Array1_cal_c[k][m-1]);		//Storing data into reference file
					}
				}
			k++;

			}
			fclose(fp);
		}

		fclose(fp_ref);

		//Creating average Ci per frame array for further calculations
		for(k=0;k<p;k++)
		{
			for(m=0;m<5;m++)
				vowel_c[i][k] += Array1_cal_c[m][k]; //Doing sum of Ci values to do average later 
			
			vowel_c[i][k] /= 5; //Doing average of Ci values 
		}
	}



	//Starting the TESTING on 10 test files of every vowel 

double wi[p] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};	//Wi values for Thokura distance
	FILE* accurate_ptr = fopen("Reference_Files/accuracydata.txt","w+");			//Making file for storing accuracy data
	for(i=0;i<5;i++)	//Looping into each vowel
	{
		int incorrect_ans=0;

		for(j=21;j<=30;j++)	//Looping into each vowel's test file , It was 11-20
		{	char c;int a=(10*i)+(j-20);
			if(a<=10)c='a';
			else if(a<=20)c='e';
			else if(a<=30)c='i';
			else if(a<=40)c='o';
			else if(a<=50)c='u';
			printf("validating %dth test file of %c\n",(j-20) ,c);
			count = 0;
			double thk_distance=0;
			double dc_shift = 0;
			double Array1_cal_c_test[p] = {0};	//Stores Ci values of test file, averaged for all samples
			
			//Creating path name for test file name o test with trained data 
			char pathname[33] = "Vowel_Files/234101049_";
			strcat(pathname,&vowels[i*2]);

			char str[3] = "";
			sprintf(str,"%d",j);

			strcat(pathname, temp_underscore);
			strcat(pathname, str);
			strcat(pathname, temp_txt);

			fp = fopen((const char *)pathname,"r");


			dc_shift=calculate_dc_shift(fp);
			rewind(fp);							//Rewinding fp to find max energy frame
			maximum_amplitude=calculate_max_amplitude(fp);
			rewind(fp);	

			double maximum_energy_of_frame = 0;
			int frame_no_with_maximum_energy = 0;
			double energy_per_frame = 0;
			int frame_no = 0;	

			while(!feof(fp))
			{
				for(k=0;k<320;k++)
				{
					fgets(sample_string, 150, fp);
					if(feof(fp)) break;

					dat = atof(sample_string);
					energy_per_frame += (dat*dat);
				}

				energy_per_frame /= k;

				if(energy_per_frame > maximum_energy_of_frame)
				{
					maximum_energy_of_frame = energy_per_frame;
					frame_no_with_maximum_energy = frame_no;
				}

				frame_no++;
			}

			rewind(fp);

			int stable_start = (frame_no_with_maximum_energy - 2)*320;

			for(k=0;k<stable_start;k++)
				fgets(sample_string,150,fp);

			int sample_size = 320;
			double values[321];
			memset(values, 0, sizeof(values));
			int returnvowel = 0;
			double min_thk_distance = 1000;	//For calculation of minimum thk_distance distance

			k=0;
			while(k<5)
			{
				for(m=1;m<=320;m++)
				{
					fgets(sample_string,150,fp);
					dat = atof(sample_string);

					dat -= dc_shift;
					dat *= (5000/maximum_amplitude);

					values[m] = dat;
				}

				double Ri[p+1];memset(Ri, 0, sizeof(Ri)); //Initializing Ri values with 0
				findRi(values, sample_size, Ri);

				double Ai[p+1];memset(Ai, 0, sizeof(Ai));//Initializing Ai values with 0
				findAi(Ri, Ai);

				double Ci[p+1];memset(Ci, 0, sizeof(Ci));//Initializing Ci values with 0

				Ci[0] = log(Ri[0]*Ri[0]);

				findCi(Ai, Ci);

				//Applying sine window to the Ci values
				for(m=1;m<=p;m++)
				{
					Array1_cal_c_test[m-1] += Ci[m]*(1+(p/2)*sin((pi*m)/p));

					if(k==4)	//Averaging for each sample in the end	
						Array1_cal_c_test[m-1] /= 5;
				}k++;
			}
			fclose(fp);

			
			//printf("Distances: ");
			k=0;
			while(k<5)
			{
				for(m=0;m<p;m++)
					thk_distance += wi[m]*(pow((Array1_cal_c_test[m] - vowel_c[k][m]),2));	//thk_distance's distance formula

				//printf("%lf\t",thk_distance);

				//Finding minimum from the distances
				if(thk_distance<min_thk_distance)
				{
					min_thk_distance = thk_distance;
					returnvowel = k;
				}

				thk_distance = 0;
				k++;
			}
			if(i != returnvowel)
				incorrect_ans++;		//Counting number of wrong prediction

			printf("\nVowel detected by system: %s\n\n",&vowels[returnvowel*2]);

						
		}
		fprintf(accurate_ptr,"Accuracy percentage of vowel %s: %d %\n", &vowels[i*2], (10*(10-incorrect_ans)));	//Save accuracy data in file
	}

	rewind(accurate_ptr);

	//Displaying accuracy data on console
	while(!feof(accurate_ptr))
	{
		fgets(sample_string,150,accurate_ptr);

		if(!feof(accurate_ptr))
			printf("%s",sample_string);
	}

	fclose(accurate_ptr);

	system("PAUSE");	//Pause console to display data

	return 0;
}


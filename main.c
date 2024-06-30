/*This code process the AUCT generated data to find the projection data of a single file.
The proj_data values are peak to peak values with outliers removed.
It take the mean of the final amplitudes.
filename: name of the data file
Select trend == 1 if you see significant trend in the data.
NOTE : Selecting the trend slowdown the timing efficiency.
@Author = ANKUR
Divyadrishti imaging lab, IIT Roorkee.
ankur89587@gmail.com; ankur@ph.iitr.ac.in
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<conio.h>
#include<string.h>
#include<errno.h>
#include <direct.h>


#include "basicfn.h"
#include "dsoCom.h"
#include "waveGenCom.h"
#include "serialCom.h"
#include "proj_dtrnd.h"
#include "reconstruct.h"
#include "gnu_plot.h"


int main()
{
	// INPUT PARAMETERS.
	char file_address[] = "E:\\Ankur\\Clang\\Test_folder";                         // Primary directory to save experiments.
	char exp_name[] = "Testing_Al";												   // Folder to save experimental data.		
	char processed_data[] = "Processed_Data";                                      // Folder to save processed data.
	int N_packets = 7;
	double distance = 22;                           // in mm.
	const char* ard_port = "\\\\.\\COM5";
	int freq = 1500000;
	int amplitude = 20;
	int NR = 10;									// Number of rotations in scan.
	int NT = 10;									// Number of linear translations.	

	int chdir_status=_chdir(file_address);
	if (chdir_status == -1)
	{
		printf("Change Directory failed.\n");
		exit(0);	
	}
	
	// Creating folder for the experimental data.
	char exp_dir[200] = { 0 };
	sprintf_s(exp_dir, 200, "%s\\%s", file_address, exp_name);

	// Creating folder for the experiment.
	int check = _mkdir(exp_dir);
	if (check == -1)
	{
		if (errno == EEXIST)
		{
			printf("Directory already exist.\n");
		}
		else
		{
			printf("Directory creation failed.\n");
		}	
	}

	// Creating folder for the processed data.
	char proc_data_dir[200] = { 0 };
	sprintf_s(proc_data_dir, 200, "%s\\%s\\%s", file_address, exp_name, processed_data);

	// Creating folder for the processed data.
	int check2 = _mkdir(proc_data_dir);
	if (check2 == -1)
	{
		if (errno == EEXIST)
		{
			printf("Directory already exist.\n");
		}
		else
		{
			printf("Directory creation failed.\n");
		}
	}

	// ................................Communicating to Pulse Generator......................................
	wavCom(freq, amplitude);
	const char pulseData[200] ={0};   // file to save pulse data.
	char pulse_dat_file_name[] = "WaveData.txt";
	sprintf_s(pulseData, 200, "%s\\%s", proc_data_dir, pulse_dat_file_name);
	writeWData(pulseData);
	//....... Connection to pulse generator automatically disconnected after triggering the pulse............

	//......................................Connecting to arduino............................................
	UINT32 baud_rate = 9600;
	HANDLE device = open_serial_port(ard_port, baud_rate);
	if (device == INVALID_HANDLE_VALUE) {
		printf("Unable to Connect to Arduino. \n");
	}
	else {
		printf("Connected to Arduino.\n");
	}
	//...................................... Connected to Arduino............................................

	//.....................................Communication to the DSO..........................................
	dsoOpen();
	//.........................................Connected to the DSO..........................................

	/* Calculations required to set the various parameters.
	* Input Data : Direction, Distanceand Speed.
	* distance = object will move within this displacement.*/

	int speed = 900;                           // fix 550 for smooth movement    upper limit 1000.
	int direction = 1;                         // 0 for Translation towards the stepper motor.
	int tmp_speed = 2500 - speed;
	int	one_mm_steps = 200;                    // Stepper steps in one mm movement.

	//During Rotation, there shouldnt be any translational motion and vice - versa
	int no_translational_steps = 0;
	int	no_rotational_steps = 0;

	/* Calculations required to set the various parameters.
	* Input Data : Direction, Distance and Speed.
	* distance = 10;% distance = object will move within this displacement.*/
	int	dis_step_size = distance / NT;
	int	total_steps_dist = one_mm_steps * distance; 
	int	total_steps_rot = 1600;                 // Total stepper steps in one rotation.


	/* Calculating the no.of steps according to the distance(in mm).
	* The threaded rod used have a pitch of 8mm / turn.
	* Also the stepper motor is moving 1600 steps per turn.steps in one rotation = 1600 */

	int	step_size_rotation = round((float)total_steps_rot / NR);    // stepper steps one in rotational step.
	//printf("step_size_rotatin = %d and actual=%f \n",step_size_rotation, (float)total_steps_rot / NR);

	int	num_translation_steps = (float)total_steps_dist / (one_mm_steps * dis_step_size);   // Total no of translational steps in the distance.
	//printf("num_translation_steps = %d and actual=%f \n", num_translation_steps, (float)total_steps_dist / (one_mm_steps * dis_step_size));

	int	step_size_translation = (float)total_steps_dist / num_translation_steps;   //Stepper steps in one translational steps.
	//printf("step_size_translation = %d and actual=%f \n", step_size_translation, (float)total_steps_dist / num_translation_steps);

	/* To format data to be sent to arduino in proper form.
	* direction = 1 char, steps = 4 char(rotation) + 4 char(translation), speed = 4 char */

	char data_to_arduino_rotation[20] = { 0 };
	sprintf_s(data_to_arduino_rotation, 20, "%d%04d%04d%04d", 0, step_size_rotation, no_translational_steps, tmp_speed);
	//printf("step_size_rotation=%d , no_translational_steps=%d,  tmp speed= %d  Data to Arduino= %s \n", step_size_rotation, no_translational_steps, tmp_speed, data_to_arduino_rotation);

	DWORD dNoOfBytesWritten = 0;
	DWORD dNoOFBytestoWrite = sizeof(data_to_arduino_rotation); // Same for both rotation and translation data.

	// To store projection data.
	int proj_size = NT * NR;
	double* proj_data = (double*)malloc(proj_size * sizeof(double));
	if (!proj_data)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(proj_data);
		return -1;
	}
	for (int j = 0; j < proj_size; j++)                    // Initialising with zeros.
	{
		proj_data[j] = 0.0;
	}
	// To save projetcion data file.
	FILE* proj_file = NULL;
	char proj_file_address[200] = { 0 };
	char* proj_file_name = "Sinogram.dat";
	sprintf_s(proj_file_address, 200, "%s\\%s", proc_data_dir, proj_file_name);
	errno_t err3 = fopen_s(&proj_file, proj_file_address, "w");
	if (!proj_file)
	{
		printf("FILE proj.txt NOT OPENED. \n");
		exit(0);
	}

	// To store projection data for each angle.
	double* proj_data_1darray = (double*)malloc(NT * sizeof(double));
	if (!proj_data_1darray)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(proj_data_1darray);
		return -1;
	}
	for (int j = 0; j < NT; j++)                    // Initialising with zeros.
	{
		proj_data_1darray[j] = 0.0;
	}
	// To store the distance travelled in the linear translations.
	double* dist_translation_array = (double*)malloc(NT * sizeof(double));
	if (!dist_translation_array)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(dist_translation_array);
		return -1;
	}

	// To store reconstruction data.
	int recon_size = NT*NT;                               // Total number of pixels (N_pixel).                   
	double* recon_data = (double*)malloc(recon_size * sizeof(double));
	if (!recon_data)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(recon_data);
		return -1;
	}
	for (int j = 0; j < recon_size; j++)                  // Initialising with zeros.
	{
		recon_data[j] = 0.0;
	}
	// To write reconstructed data to file.
	FILE* recon_file = NULL;
	char recon_file_address[200] = { 0 };
	char* recon_file_name = "reconstruction.dat";
	sprintf_s(recon_file_address, 200, "%s\\%s", proc_data_dir, recon_file_name);
	errno_t err4 = fopen_s(&recon_file, recon_file_address, "w");
	if (!recon_file)
	{
		printf("FILE proj_write NOT OPENED. \n");
		exit(0);
	}


	char* channel = "C1";                                // Data acquisition channel number.
	
	//...........................................THE SCANNING LOOP...............................................
	for (int rot = 0; rot < NR; rot++)          // Rotation Loop
	{
		int a = pow((-1), (rot + 1));
			if (a == 1)
			{
				direction = 0;
			}
			else
			{
				direction = 1;
			}
			char data_to_arduino_translation[20] = { 0 };
			sprintf_s(data_to_arduino_translation, 20, "%d%04d%04d%04d", direction, no_rotational_steps, step_size_translation, tmp_speed);

			// Linear Translation Loop.
			for (int dis = 0; dis < NT; dis++)              // Translation Loop
			{
				BOOL Status = WriteFile(device, data_to_arduino_translation, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL); // Send the data to arduino in the required format.
				waitFor(0.02);                    // Sleep time in seconds.

				dist_translation_array[dis] = dis_step_size * dis;
				//printf("dis_arr= %e \n",dist_translation_array[dis]);

				// Acquiring the data from DSO.
				char f_name[200] = { 0 };
				sprintf_s(f_name, 200, "%s\\%d_%d.txt", exp_dir, rot, dis);
				dsoCom(channel, f_name);                                        // Acquired data will be saved to f_name file.

				// To process the acquired data.
				double proj = 0;
				proj_dtrnd(f_name, N_packets, 0, NULL, 0, &proj);               
				//printf("proj data= %lf \n", proj);
				if (a == -1)
				{
					proj_data[rot * NT + dis] = proj;                          // For forward direction.
					proj_data_1darray[dis] = proj;
				}
				else                                                           // Rotations are alterante.
				{
					proj_data[(rot + 1) * NT - dis-1] = proj;					   // For reverse direction, towards stepper motor.
					proj_data_1darray[NT-dis-1] = proj;
				}
			}
			// End of linear translation loop

			// Data to arduino to rotate the object.
			BOOL Status = WriteFile(device, data_to_arduino_rotation, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL);    // writing the data to the serial port
			//printf("\n Status= %d \n", Status);
			waitFor(0.01);                    // Sleep time in seconds.

			// Visualisation .
			gnuplot_open();
			// Image 1, The amplitude attenuation while object is translating linearly.
			char* xlabel = "Distance"; char* ylabel = "Amplitude"; char* title = "Amplitude Attenuation";
			plot_graph(dist_translation_array, proj_data_1darray,  NT, xlabel, ylabel, title);
			
			// Squaring and Normalising the sinogram.
			double max_proj_data = maximum(proj_data, proj_size);
			for (int n = 0; n < proj_size; n++)
			{
				proj_data[n] = pow(proj_data[n], 2)/max_proj_data;
			}
			char* sino_title = "Sinogram";
			plot_matrix(1, proj_data, NR, NT, sino_title);

			// Reconstruction.
			int method = 1;                   // MART 1, MART 2, MART 3.
			reconstruct(NR, NT, proj_data, proj_size, method, recon_data);
			// Normalising the reconstruction data.
			double max_recon_data = maximum(recon_data, recon_size);
			for (int n = 0; n <recon_size; n++)
			{
				recon_data[n] = pow(recon_data[n], 2) / max_recon_data;
			}
			char* recon_title = "Reconstruction";
			plot_matrix(2, recon_data, NT, NT, recon_title);
	}

	//............................. ......End of rotation and scanning loop............................................................
	char data_to_home_pos[20] = {0};
	if (pow((-1.0), NR) != 1)
	{
		if (direction == 1)
		{
			direction = 0;
		}
		sprintf_s(data_to_home_pos, 20, "%d%04d%04d%04d", direction, no_rotational_steps, NT*step_size_translation, tmp_speed);
	}
	/*
	DWORD dNoOFBytestoWrite = sizeof(data_to_arduino_rotation);
	DWORD dNoOfBytesWritten = 0;

	BOOL Status = WriteFile(device, data_to_arduino_rotation, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL);    // writing the data to the serial port
	printf("\n Status= %d \n", Status);
	*/

	// Saving the projection data and the reconstruction data.
	for (int j = 0; j < NR; j++)    // Projection/sinogram data.
	{
		for (int k = 0; k < NT; k++)
		{
			fprintf(proj_file, "%e\t", proj_data[j * NT + k]);
			//printf("%.6e \t", proj_data[j * NT + k]);
		}
		fprintf(proj_file, "\n");
	}

	for (int j = 0; j < NT; j++)     // Reconstruction data.
	{
		for (int k = 0; k < NT; k++)
		{
			fprintf(recon_file, "%e\t", recon_data[j * NT + k]);
		}
		fprintf(recon_file, "\n");
	}

	////Closing the connections.
	CloseHandle(device);    // closing the serial port.

	// Closing the connection to DAQ system.
	dsoClose();

	// Closing the gnuplot connection. 
	gnuplot_close();

	// Closing the opened files.
	fclose(proj_file);
	fclose(recon_file);

	// Clear the allocated memory.
	free(proj_data);
	free(proj_data_1darray);
	free(recon_data);

}
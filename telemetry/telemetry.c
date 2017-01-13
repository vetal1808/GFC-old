#include "telemetry.h"
#include "stab_alg.h"

void tx_update(vector4 summary_quatern, uint16_t computing_time){
	static uint8_t skip_counter = 0;
	const uint8_t skip_num = 2;
	if(skip_counter<skip_num){
		skip_counter++;
		return;
	}
	skip_counter = 0;
	vector3_int16 euclid_angles;
	euclid_from_quatern(summary_quatern, &euclid_angles);
    int16_t tmp_array[rx_channal_num];
	tmp_array[0] = euclid_angles.x*6;
	tmp_array[1] = euclid_angles.y*6;
	tmp_array[2] = euclid_angles.z*3;
	tmp_array[3] = (int16_t)(Ox.x*200.0);
	tmp_array[4] = (int16_t)(Ox.y*200.0);
	tmp_array[5] = (int16_t)(Ox.z*200.0);
	tmp_array[6] = (int16_t)(Oy.x*200.0);
	tmp_array[7] = (int16_t)(Oy.y*200.0);
	tmp_array[8] = (int16_t)(Oy.z*200.0);
	tmp_array[9] = (int16_t)(Oz.x*200.0);
	tmp_array[10] = (int16_t)(Oz.y*200.0);
	tmp_array[11] = (int16_t)(Oz.z*200.0);


	tmp_array[12] = computing_time;
    load_tx_buffer(tmp_array);
	transmit_masked_channal();
}

void rx_update(vector4 * RC_quaternion, vector3 * gyro_shift, float * avarage_thrust, uint8_t * motor_mask){
    receive_all_available();
    int16_t rx_array[rx_channal_num];
    get_rx_buffer(rx_array);
	
	set_P_gain((float)(rx_array[3]*4));
	set_I_gain((float)(rx_array[4]*4));
	set_D_gain((float)(rx_array[5]));
	set_P_limit((float)(rx_array[6]));
	set_I_limit((float)(rx_array[7]));
	set_D_limit((float)(rx_array[8]));
	*motor_mask = rx_array[10];
	set_tx_mask(rx_array[11]);
	add_to_channal(-1,12);

	if (rx_array[12]<1)
	{
		//if lose connection with pilot set half-thrust and zero angles
		*avarage_thrust = (float)(rx_array[9]/2);
		rx_array[0] = 0;
		rx_array[1] = 0;
	}
	else
		*avarage_thrust = (float)(rx_array[9]);
	calculate_RC_angles((vector3_int16){rx_array[0], rx_array[1], rx_array[2]}, RC_quaternion, gyro_shift);



	/*
		rx_array[10] - motor_mask
		rx_array[11] - transmit_mask
		rx_array[12] - lose connection
		
	*/
}

void calculate_RC_angles(vector3_int16 RC_raw_data, vector4 * return_quaternion, vector3 * gyro_shift){
	static float RC_yaw = 0.0;
	static vector3_int16 RC_raw_data_prev = {0, 0, 0};
	
	vector3_int16 delta;
	delta.x = RC_raw_data.x - RC_raw_data_prev.x;
	delta.y = RC_raw_data.y - RC_raw_data_prev.y;
	static int16_t limit = 20;
	limit_value16( &delta.x, limit);
	limit_value16( &delta.y, limit);
	RC_raw_data_prev.x += delta.x;
	RC_raw_data_prev.y += delta.y;

	float delta_yaw = RC_raw_data.z*minuteArc_to_rad;
	RC_yaw += delta_yaw *update_period_in_sec;
	
	vector3 RC_angles;
	RC_angles.x = RC_raw_data_prev.x*minuteArc_to_rad;
	RC_angles.y = RC_raw_data_prev.y*minuteArc_to_rad;
	RC_angles.z = RC_yaw;
	*return_quaternion = euclid_to_quaterion(RC_angles);

	gyro_shift->x = delta.x*minuteArc_to_rad*frq;
	gyro_shift->y = delta.y*minuteArc_to_rad*frq;
	gyro_shift->z = delta_yaw;
}

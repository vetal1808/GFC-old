#include "telemetry.h"
#include "stab_alg.h"
void tx_update(){
	static uint8_t skip_counter = 0;
	const uint8_t skip_num = 2;
	if(skip_counter<skip_num){
		skip_counter++;
		return;
	}
	skip_counter = 0;
	transmit_masked_channal();
}


void rx_update(){
    receive_all_available();
	uint16_t tmp[2];
	get_rx_buffer(tmp, TX_MASK, 2);
	uint32_t tmp2 = tmp[0]<<16 | tmp[1];
	set_tx_mask(tmp2);
	add_to_channal(-1,LOST_CONNECTION_COUNTER);

}

int8_t get_RC_state(vector4 * RC_quaternion, vector3 * RC_spin, int16_t * thrust){
	static float RC_yaw = 0.0;
	static vector3_int16 RC_raw_data_prev = {0, 0, 0};
	
	vector3_int16 RC_raw_data;
	get_rx_buffer((int16_t *)&RC_raw_data, RC_ANGLES, 3);
	get_rx_buffer(thrust, THRUST, 1);
	int16_t is_RC_connected;
	get_rx_buffer(&is_RC_connected, LOST_CONNECTION_COUNTER, 1);
	if (is_RC_connected<1)
	{
		//if lose connection with pilot set half-thrust and zero angles
		*thrust /= 2;
		RC_raw_data.x = 0;
		RC_raw_data.y = 0;
	}
	
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
	*RC_quaternion = euclid_to_quaterion(RC_angles);

	RC_spin->x = delta.x*minuteArc_to_rad*frq;
	RC_spin->y = delta.y*minuteArc_to_rad*frq;
	RC_spin->z = delta_yaw;
	if (is_RC_connected>0)
		return 1;
	else
		return 0;
}

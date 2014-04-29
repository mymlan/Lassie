#include <stdint.h>

// gör om till mm Hugo!

 uint8_t S1_convert_sensor_value_left_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 135 && digital_distance >= 114)
	{
		mm_value = (((digital_distance - 135)*10) / -21);
	}
	else if (digital_distance <=114 && digital_distance >= 97)
	{
		mm_value = (((digital_distance - 131)*10) / -17);
	}
	else if (digital_distance <= 97 && digital_distance >= 77)
	{
		mm_value = ((digital_distance - 117) / -1);
	}
	else if (digital_distance <= 77 && digital_distance >= 62)
	{
		mm_value = (((digital_distance - 107)*100) / -75);
	}
	else if (digital_distance <= 62 && digital_distance >= 50)
	{
		mm_value = (((digital_distance - 86)*10) / -4);
	}
	else if (digital_distance <= 50 && digital_distance >= 35)
	{
		mm_value = (((digital_distance - 77)*10) / -3);
	}
	else if (digital_distance <= 35 && digital_distance >= 26)
	{
		mm_value = (((digital_distance - 56)*100) / -15);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S2_convert_sensor_value__left_back(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 130 && digital_distance >= 112)
	{
		mm_value = (((digital_distance - 130)*10) / -18);
	}
	else if (digital_distance <=112 && digital_distance >= 96)
	{
		mm_value = (((digital_distance - 128)*10) / -16);
	}
	else if (digital_distance <= 96 && digital_distance >= 73)
	{
		mm_value = (((digital_distance - 119)*100) / -115);
	}
	else if (digital_distance <= 73 && digital_distance >= 58)
	{
		mm_value = (((digital_distance - 103)*100) / -75);
	}
	else if (digital_distance <= 58 && digital_distance >= 41)
	{
		mm_value = (((digital_distance - 92)*100) / -57);
	}
	else if (digital_distance <= 41 && digital_distance >= 26)
	{
		mm_value = (((digital_distance - 68)*10) / -3);
	}
	else if (digital_distance <= 26 && digital_distance >= 18)
	{
		mm_value = (((digital_distance - 45)*100) / -13);
	}
	else
	{
		mm_value = 250;
	}
	return mm_value;
}

uint8_t S3_convert_sensor_value_right_front(uint8_t digital_distance)
{
	uint8_t mm_value;
	
	if (digital_distance <= 148 && digital_distance >= 125)
	{
		mm_value = ((digital_distance-240)/-23);
	}
	else if (digital_distance <=125 && digital_distance >= 109)
	{
		mm_value = ((digital_distance-205)/-16);
	}
	else if (digital_distance <= 109 && digital_distance >= 94)
	{
		mm_value = ((digital_distance-199)/-15);
	}
	else if (digital_distance <= 94 && digital_distance >= 70)
	{
		mm_value = ((digital_distance-150)/-8);
	}
	else if (digital_distance <= 70 && digital_distance >= 49)
	{
		mm_value = ((digital_distance-111)/-4.1);
	}
	else if (digital_distance <= 49 && digital_distance >= 34)
	{
		mm_value = ((digital_distance-94)/-3);
	}
	else
	{
		mm_value = 25;
	}
	return mm_value;
}

uint8_t S4_convert_sensor_value_right_back(uint8_t digital_distance)
{
	int mm_value;
	
	if (digital_distance <= 149 && digital_distance >= 124)
	{
		mm_value = ((digital_distance-249)/-25);
	}
	else if (digital_distance <=124 && digital_distance >= 107)
	{
		mm_value = ((digital_distance-209)/-17);
	}
	else if (digital_distance <= 107 && digital_distance >= 94)
	{
		mm_value = ((digital_distance-185)/-13);
	}
	else if (digital_distance <= 94 && digital_distance >= 68)
	{
		mm_value = ((digital_distance-154.66)/-8.66);
	}
	else if (digital_distance <= 68 && digital_distance >= 48)
	{
		mm_value = ((digital_distance-108)/-4);
	}
	else if (digital_distance <= 48 && digital_distance >= 36)
	{
		mm_value = ((digital_distance-84)/-2.4);
	}
	else
	{
		mm_value = 25;
	}
	return mm_value;
}

uint8_t S5_convert_sensor_value_front_long(uint8_t digital_distance)
{
	int cm_value;
	
	if (digital_distance <= 130 && digital_distance >= 92)
	{
		cm_value = ((digital_distance-206)/-7.6);
	}
	else if (digital_distance <= 92 && digital_distance >= 72)
	{
		cm_value = ((digital_distance-152)/-4);
	}
	else if (digital_distance <= 72 && digital_distance >= 60)
	{
		cm_value = ((digital_distance-117)/-2.4);
	}
	else if (digital_distance <= 60 && digital_distance >= 53)
	{
		cm_value = ((digital_distance-95)/-1.4);
	}
	else if (digital_distance <= 53 && digital_distance >= 35)
	{
		cm_value = ((digital_distance-86)/-1.1);
	}
	else if (digital_distance <= 35 && digital_distance >= 30)
	{
		cm_value = ((digital_distance-50)/-0.25);
	}
	else
	{
		cm_value = 100;
	}
	return cm_value;
}
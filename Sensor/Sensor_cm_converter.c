/*
 * CFile1.c
 *
 * Created: 2014-04-16 13:50:22
 *  Author: Hugo
 */ 

 int S1_sensor_value_front(int n)
{
	int cm_value;
	
	if (n <= 146 && n >= 105)
	{
		cm_value = ((n-228)/-20.5);
	}
	else if (n <=105 && n >= 93)
	{
		cm_value = ((n - 186)/-13.5);
	}
	else if (n <= 93 && n >= 66)
	{
		cm_value = ((n-156)/-9);
	}
	else if (n <= 66 && n >= 40)
	{
		cm_value = ((n-112)/-4.8);
	}
	else if (n <= 40 && n >= 34)
	{
		cm_value = ((n-58)/-1.2);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

int S2_sensor_value_front(int n) 
{
	int cm_value;
	
	if (n <= 148 && n >= 125)
	{
		cm_value = ((n-240)/-23);
	}
	else if (n <=125 && n >= 109)
	{
		cm_value = ((n-205)/-16);
	}
	else if (n <= 109 && n >= 94)
	{
		cm_value = ((n-199)/-15);
	}
	else if (n <= 94 && n >= 70)
	{
		cm_value = ((n-150)/-8);
	}
	else if (n <= 70 && n >= 49)
	{
		cm_value = ((n-111)/-4.1);
	}
	else if (n <= 49 && n >= 34)
	{
		cm_value = ((n-94)/-3);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

int S3_sensor_value_front(int n)
{
	int cm_value;
	
	if (n <= 148 && n >= 125)
	{
		cm_value = ((n-240)/-23);
	}
	else if (n <=125 && n >= 109)
	{
		cm_value = ((n-205)/-16);
	}
	else if (n <= 109 && n >= 94)
	{
		cm_value = ((n-199)/-15);
	}
	else if (n <= 94 && n >= 70)
	{
		cm_value = ((n-150)/-8);
	}
	else if (n <= 70 && n >= 49)
	{
		cm_value = ((n-111)/-4.1);
	}
	else if (n <= 49 && n >= 34)
	{
		cm_value = ((n-94)/-3);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

int S4_sensor_value_front(int n)
{
	int cm_value;
	
	if (n <= 149 && n >= 124)
	{
		cm_value = ((n-249)/-25);
	}
	else if (n <=124 && n >= 107)
	{
		cm_value = ((n-209)/-17);
	}
	else if (n <= 107 && n >= 94)
	{
		cm_value = ((n-185)/-13);
	}
	else if (n <= 94 && n >= 68)
	{
		cm_value = ((n-154.66)/-8.66);
	}
	else if (n <= 68 && n >= 48)
	{
		cm_value = ((n-108)/-4);
	}
	else if (n <= 48 && n >= 36)
	{
		cm_value = ((n-84)/-2.4);
	}
	else
	{
		cm_value = 25;
	}
	return cm_value;
}

int S5_sensor_value_long(int n)
{
	int cm_value;
	
	if (n <= 130 && n >= 92)
	{
		cm_value = ((n-206)/-7.6);
	}
	else if (n <= 92 && n >= 72)
	{
		cm_value = ((n-152)/-4);
	}
	else if (n <= 72 && n >= 60)
	{
		cm_value = ((n-117)/-2.4);
	}
	else if (n <= 60 && n >= 53)
	{
		cm_value = ((n-95)/-1.4);
	}
	else if (n <= 53 && n >= 35)
	{
		cm_value = ((n-86)/-1.1);
	}
	else if (n <= 35 && n >= 30)
	{
		cm_value = ((n-50)/-0.25);
	}
	else
	{
		cm_value = 100;
	}
	return cm_value;
}
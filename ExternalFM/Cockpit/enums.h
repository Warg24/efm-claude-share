#pragma once
//#include <ed/map.h>
//#include <osg/Vec3f>

enum cockpit_args
{

};

enum external_args
{

};

enum class dsp_mode
{
	blank = -1,
	main_menu = 0,
	Map = 1,
	NavRadar = 2,
	Taws = 3,
};

enum class display_side
{
	pilot = 0,
	instructor = 2,
};

enum class display_pos
{
	front = 0,
	rear = 1,
};
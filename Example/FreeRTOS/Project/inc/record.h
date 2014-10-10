#ifndef record_h
#define record_h
//空调各种状态信息结构体
struct air_condition
{
	char id;//设备号
	
	char month;//月
	char week;//周几
	char day;//日
	char hour;//小时
	char minute;//分钟

	char out_temp;//室外温度
	char in_temp;//室内温度
	char air_temp;//空调温度
	char set_temp;//设置温度

	char out_mois;//室外湿度
	char in_mois;//室内湿度

	char air_windV;//空调风速
	char set_windV;//设置风速

	char air_windUD;//空调上下风向
	char set_windUD;//设置风向

	char air_windLR;//空调左右风向
	char set_windLR;//设置风向

	char air_mode;//空调模式
	char set_mode;//设置模式

	char air_scene;//空调场景
	char set_scene;//设置场景

	char air_status;//是否开关机
	char set_status;//是否开关机
	
	char is_person;//是否有人
	char air_fault; //发生故障
};
#endif // record_h
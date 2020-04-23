//------------------------------------------------------------------
#ifndef SOCKET_STRUCT_WIDE_H
#define SOCKET_STRUCT_WIDE_H
//------------------------------------------------------------------
// #ifdef __uClinux__
// #include <sys/time.h>
// #else
// #include <winsock2.h>
// #endif
//------------------------------------------------------------------
//����� �� �������
#define CMD_ANSWER						1		
//------------------------------------------------------------------
//������� �������
#define CMD_SET_TIME					2		//���������� ����� �� ����������
#define CMD_SHUTDOWN					3		//��������� ����������
#define CMD_RESET						4		//������������� ����������
//------------------------------------------------------------------
//������� HAL�
#define CMD_HAL_INIT					5		//������������� HAL
#define CMD_HAL_EXIT					6		//���������� ������ � HAL
#define CMD_HAL_RESET					7		//������������ HAL
#define CMD_HAL_TEST					8		//�������� ������� ���. ����������
#define CMD_HAL_GET_DESC				9		//�������� ����������
#define CMD_HAL_START_LOG				10		//��������� ������ ����������������
#define CMD_HAL_STOP_LOG				11		//���������� ������ ����������������
#define CMD_HAL_START_AUTOCALIB			12		//��������� ������ ��������������
#define CMD_HAL_GET_KOEFS				14		//�������� ������� ������������ ��������������
#define CMD_HAL_ADD_FREQ_MONITOR		15		//�������� ������ ����������� ��������
#define CMD_HAL_ADD_STAT_MONITOR		16		//�������� ������ ����������� �������
#define CMD_HAL_START_MONITOR			17		//��������� ������ �����������
#define CMD_HAL_STOP_MONITOR			18		//���������� ������ �����������
#define CMD_HAL_DEL_MONITOR				19		//������� ������ �����������
#define CMD_HAL_START_AUDIO				20		//�������
#define CMD_HAL_STOP_AUDIO				21		//�� �������
//------------------------------------------------------------------
//���������� ������� DSA
#define CMD_HAL_ADD_DSA_TASK				30	//�������� ������ DSA
#define CMD_HAL_ADD_DSA_CHANNEL_HSMF		31	//�������� ����� HSMF
#define CMD_HAL_ADD_DSA_CHANNEL_FREQ		32	//�������� ����� ������� ��������
#define CMD_HAL_ADD_DSA_SUBTASK_SPECTRUM	33	//�������� � ������ DSA ��������� ���������� �������
#define CMD_HAL_ADD_DSA_SUBTASK_CALC		34	//�������� � ������ DSA ��������� ���������� ��������� ���������� �������
#define CMD_HAL_ADD_DSA_SUBTASK_KURTOSIS	35	//�������� � ������ DSA ��������� ���������� ��������� � ������ ��� ���������
#define CMD_HAL_ADD_DSA_SUBTASK_STD_INTEG	36	//�������� � ������ DSA ��������� ���������� ��� ���������������� �������
#define CMD_HAL_ADD_DSA_SUBTASK_VECTOR_FROT	37	//�������� � ������ DSA ��������� ���������� ������� ��������
#define CMD_HAL_ADD_DSA_SUBTASK_TIMESIGNAL	38  //�������� � ������ DSA ��������� ����� ���������� �������
#define CMD_HAL_START_DSA_TASK				40	//��������� ������ DSA
#define CMD_HAL_STOP_DSA_TASK				41	//���������� ������ DSA
#define CMD_HAL_DEL_DSA_TASK				42	//������� ������ DSA
#define CMD_HAL_START_TS_TASK				43  //��������� ��������� ����� ���������� �������
#define CMD_HAL_STOP_TS_TASK				44  //���������� ��������� ����� ���������� �������
#define CMD_HAL_GET_DESC_2					1000 //descriptor_2  //общая информация об изделии
#define CMD_SET_IP							1002
#define CMD_CALIBRATION_INFO				1004  //информация о поверке
//------------------------------------------------------------------
//�������� ��������� �� ��������� �������������
#define CALLBACK_LOG_MESSAGE			114		//��������� �� ������� (������ ����������������)
#define CALLBACK_LOG_UNCONNECT			115		//��������� �� ������� (��� ��������)
#define CALLBACK_AUTOCALIB_PROGRESS		116		//�������� ���������� ��������������
#define CALLBACK_AUTOCALIB_UNCONNECT	117		//�������� ���������� ��������������
#define CALLBACK_MONITOR_DATA			118		//������
#define CALLBACK_MONITOR_UNCONNECT		119		//����������
#define CALLBACK_MONITOR_STAT_DATA		120		//������
#define CALLBACK_MONITOR_STAT_UNCONNECT	121		//����������
#define CALLBACK_AUDIO_UNCONNECT		122		//���������� ���������
#define CALLBACK_DSA_PROGRESS			130		//������� ���������� ������ DSA
#define CALLBACK_DSA_UNCONNECT			131		//���������� ������ ������ DSA
#define CALLBACK_TS_PROGRESS			140		//������� ������ ������ TS
#define CALLBACK_TS_UNCONNECT			141		//���������� ������ ������ TS

//������ ������� 3,4
#define CALLBACK_TS_FILE_DESC			150		//�������� ����������
#define CALLBACK_TS_FILE_BUFFER			151		//�������� �����
#define CALLBACK_TS_FILE_CLOSE			152		//����� �����
//������ ������� 5
#define CALLBACK_TS_BEGIN				153		//������ ������ �������
#define CALLBACK_TS_BUFFER				154		//����� �������
#define CALLBACK_TS_END					155		//����� ������ �������
#define CALLBACK_HAL_DESC_2				1001	//descriptor_2  //общая информация об изделии
#define CALLBACK_SET_IP					1003
#define CALLBACK_CALIBRATION_INFO		1005	//информация о поверке
//------------------------------------------------------------------
//��������������� ����������� HAL
#ifndef CHECK_EC
#define CHECK_EC(ErrCode)           ((ErrCode) >= 0)
#endif
// ������� ��������� �������
#define TCP_EC_SUCCESS                  0
// ���������� ������
#define TCP_EC_DEV_IS_OPEN              1
// ������� ���������, ��� ��������� �������
#define TCP_EC_UNSUCCESS                -1
// ������� �� �����������, ������������ ��� �������
#define TCP_EC_BUG                      -2
// ������ ������� �� �������������� ��� ������� ����������
#define TCP_EC_NOT_SUPPORT              -3
// �������� ������� ���������
#define TCP_EC_INV_PARAMS               -4
// ��� ����� ������
#define TCP_EC_NO_SUCH_TASK             -5
// ������ ��� �����������
#define TCP_EC_TASK_IN_PROCESS          -6
// �� ����� ���������� ������ ��� �������� ����� ����������
#define TCP_EC_RESET                    -7
// ������ ���� ��������
#define TCP_EC_CANCELLED                -8
// ��������� �� �����������
#define TCP_EC_SUBTASK_NEVER_RUN        -9
// ���������� �� �������� �� �������
#define TCP_EC_DEV_NOT_RESPOND			-10
// �� ���� ��������� ���������� � ������� ��������
#define TCP_EC_WRONG_ROTATE				-11
// ������������ ������
#define TCP_EC_DATA_TOO_LITTLE			-12
// ������ �� ����������������
#define TCP_EC_SPECT_NOT_STABLE			-13
// ����� ������ �����
#define TCP_EC_CHANNEL_BUSY				-14
// ���������� ���������
#define TCP_EC_DISABLED					-15
// ��� ������ ��� ���������� ��������
#define TCP_EC_NO_MEMORY				-16
// ���������� ����������
#define TCP_EC_DEV_BROKEN				-17
// Sensor is unconnected
#define TCP_EC_SENSOR_UNCONNECTED		-18
//����������� ��������� ��������
#define TCP_EC_NOT_RESULT				-255
//������ ��������� �����������
#define TCP_EC_ERROR_DESCRIPTOR			-256
//��� ����� ������
#define TCP_EC_NOT_TASK					-257
//------------------------------------------------------------------
//���������, ���������� �� �������� ���������� ������������ ��� ��������� �������
//�������� �����
#define SENSOR_DCMIN_DUMMY                    0.0
#define SENSOR_DCMAX_DUMMY                    0.0
//��� ������
#define SENSOR_DCMIN_ACCELEROMETER_VOLTAGE    3.0
#define SENSOR_DCMAX_ACCELEROMETER_VOLTAGE    12.0
//��� ������ ICP
#define SENSOR_DCMIN_ACCELEROMETER_ICP        7.0
#define SENSOR_DCMAX_ACCELEROMETER_ICP        15.0
//��� ��������� �����1
#define SENSOR_DCMIN_LINE_MINUS_10_PLUS_10	  0.0
#define SENSOR_DCMAX_LINE_MINUS_10_PLUS_10    0.0
//��� ��������� �����2
#define SENSOR_DCMIN_LINE_0_PLUS_20           0.0
#define SENSOR_DCMAX_LINE_0_PLUS_20           0.0
//------------------------------------------------------------------
//���������� ��������������
#define TCP_CALIB_NOT_CHECKED			0
#define TCP_CALIB_NOT_SUPPORTED			1
#define TCP_CALIB_VALID					2
#define TCP_CALIB_INVALID				3
//------------------------------------------------------------------
//��������� ������ ��� ������ ��������
#define TCP_SENSOR_DUMMY		            0
#define TCP_SENSOR_ACCELEROMETER_VOLTAGE    1
#define TCP_SENSOR_ACCELEROMETER_ICP        2
#define TCP_SENSOR_LINE_MINUS_10_PLUS_10	3
#define TCP_SENSOR_LINE_0_PLUS_20			4
//------------------------------------------------------------------
//��������� ��� �������
//�������� 1 .. 100 ��
#define TCP_DIAPAZON_1_100					1
//�������� 1 .. 1000 ��
#define TCP_DIAPAZON_1_1000					2
//�������� 0,001 .. 7 �			
#define TCP_DIAPAZON_0001_7					3
//�������� -9 .. +9 �
#define TCP_DIAPAZON_9_9					4
//�������� +1 .. 19 �
#define TCP_DIAPAZON_1_19					5
//------------------------------------------------------------------
// ����������� �������� ����������� �������������
#define TCP_GSM_AUTO                    1
// ����������� �������� �������� � �������
#define TCP_GSM_MANUAL                  2
// ����������� �������� �����������, �������� ������ ������� ������ �.�. ���������� ���������
#define TCP_GSM_MINIMUM_WITH_DC			3
//------------------------------------------------------------------
//���� ��������
#define TCP_SUBTASK_TYPE_UNKNOWN	    0
#define TCP_SUBTASK_TYPE_SPECTRUM       1
#define TCP_SUBTASK_TYPE_TIMESIGNAL     2
#define TCP_SUBTASK_TYPE_CALC           3
#define TCP_SUBTASK_TYPE_KURTOSIS       4
#define TCP_SUBTASK_TYPE_VECTOR_FROT    5
#define TCP_SUBTASK_TYPE_SYNCA          6
#define TCP_SUBTASK_TYPE_STD_INTEG      7
//------------------------------------------------------------------
//��������� ������ TCP
typedef struct 
{
	uint32_t command;
	uint32_t length;
	char* buff;
}tcp_packet_t;
//------------------------------------------------------------------
//��������� ��������� ������ �� ������ ���������� �������
typedef struct 
{
	uint16_t channel; //����� (0-3)
	uint16_t subchannel; //����� �������������� (0-7)
	uint16_t freq_channel; //����� �� 0,1
	uint16_t sensor; //����� ������� TCP_SENSOR_
	uint16_t secs; //����� ������ � ��������
	uint16_t gsm; //��� ������ ������������ �������� TCP_GSM_XXX
	float req_gain; //����������� �������� ��� ������� ������ �������
}ts_channel_t;
//��������� ���� ������� �� ������ ���������� �������
typedef struct
{
	uint16_t begin_pause; //��������� ����� ����� �������
	uint16_t num_ch; //����� ������� ��� ������ (�� ������ 4�)
	ts_channel_t* ch; //������ �������� �������
}ts_task_t;
//------------------------------------------------------------------
//���������� ���������� (�����)
typedef struct 
{
	uint16_t id;
	uint16_t dev_lib_version;
	uint16_t interface_version;
	uint16_t server_version;
	uint32_t len_dev_name;
	char* dev_name;
}tcp_hal_desc_t;
//------------------------------------------------------------------
//++added  CMD_HAL_GET_DESC_2					1000/1001
//общая информация об изделии
typedef struct 
{
	uint32_t id;				//автоикремент
	char[12] serial_number; 	//заводской номер устройства (строка)
	char[12] manufacture_date;	//дата изготовления (строка);
	char[128] manufacturer; 	//завод изготовитель (строка)
	char[12] ver_mbed_os;       // mbed os version (строка);
	char[12] ver_firmware;  	//  (строка);

	// dev_lib_version;
	// uint16_t interface_version;
	// uint16_t server_version;
	// uint32_t len_dev_name;
	// char* dev_name;
}tcp_hal_desc_t2;
//------------------------------------------------------------------
//++added  CMD_SET_IP							1002/1003
typedef struct 
{
	char[16] ip;
	char[16] netmask;
	char[16] gateway;
	char[16] port;
}tcp_set_ip;
//------------------------------------------------------------------
//added  CALLBACK_CALIBRATION_INFO		1005	//информация о поверке
//информация о поверке
typedef struct {
	uint32_t id; 						// № stm 
	char[12] serial_number;        		//заводской номер устройства (строка);
	char[12] verification_date;  		//дата поверки/калибровки (строка);
	char[12] verification_end;  		//дата окончания поверки/калибровки (строка);
	char[32] certificate_number;  		// номер сертификата;
	char[128] organization_certificate	//организация, выдавшая сертификат
}tcp_calibration_info;

//------------------------------------------------------------------
//��������� ��������� ������������� �������
typedef struct
{	
	uint16_t channel; //����� (0-3)
	uint16_t subchannel; //������������� (0-7)
	uint16_t sensor; //�����
	short volume; //���������
	float sensitivity; //����������������
	float dc_min; //����������� �������� ��������� ��� �������� �������������� �������
	float dc_max; //������������ �������� ��������� ��� �������� �������������� �������
}audio_task_t;
//------------------------------------------------------------------
//��������� ��������� ������ HSMF ��� ������ DSA
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t channel; //����� (0-3)
	uint16_t subchannel; //����� �������������� (0-7)
	uint16_t sensor; //����� TCP_SENSOR_XXX
	uint16_t gsm; //��� ������ ������������ �������� TCP_GSM_XXX
	float req_gain; //����������� �������� ��� ������� ������ �������
	float sensitivity; //����������������
	float dc_min; //����������� ���������
	float dc_max; //������������ ���������
}hsmf_channel_t;
//------------------------------------------------------------------
//��������� ��������� ������ ������� �������� ��� ������ DSA
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t channel; //����� �� 1,2
	short labels; //���������� �����
}freq_channel_t;
//------------------------------------------------------------------
//�������������� ����� �������
#define TCP_SPECTRUM_TYPE_INVALID       0
#define TCP_SPECTRUM_TYPE_DIRECT        1	//������
#define TCP_SPECTRUM_TYPE_ENVELOPE      2	//���������
#define TCP_SPECTRUM_TYPE_DIR_INTEG     3	//��������������� ������
//���
#define LPF_INVALID	0
#define LPF_25		1
#define LPF_50		2
#define LPF_100		3
#define LPF_200		4
#define LPF_400		5
#define LPF_800		6
#define LPF_1600	7
#define LPF_3200	8
#define LPF_6400	9
#define LPF_12800	10
#define LPF_25600	11
//��
#define BPF_INVALID	0
//����� ��������
#define BPF_1_3_800		1	//����������� 800 ��, ������ 185��
#define BPF_1_3_1000	2	//����������� 1000 ��, ������ 232 ��
#define BPF_1_3_1250	3	//����������� 1230 ��, ������ 289 ��
#define BPF_1_3_1600	4	//����������� 1600 ��, ������ 371 ��
#define BPF_1_3_2000	5	//����������� 2000 ��, ������ 463 ��
#define BPF_1_3_2500	6	//����������� 2500 ��, ������ 579 ��
#define BPF_1_3_3200	7	//����������� 3200 ��, ������ 741 ��
#define BPF_1_3_4000	8	//����������� 4000 ��, ������ 926 ��
#define BPF_1_3_5000	9	//����������� 5000 ��, ������ 1158 ��
#define BPF_1_3_6400	10	//����������� 6400 ��, ������ 1482 ��
#define BPF_1_3_8000	11	//����������� 8000 ��, ������ 1853 ��
#define BPF_1_3_10000	12	//����������� 10000 ��, ������ 2316 ��
#define BPF_1_3_12800	13	//����������� 12800 ��, ������ 2964 ��
#define BPF_1_3_16000	14	//����������� 16000 ��, ������ 3705 ��
#define BPF_1_3_20000	15	//����������� 20000 ��, ������ 4631 ��
//��������
#define BPF_1_1_3750	16	//����������� 3750 ��, ������ 2652 ��
#define BPF_1_1_7500	17	//����������� 7500 ��, ������ 5303 ��
#define BPF_1_1_15000	18	//����������� 15000 ��, ������ 10606 ��
//����������� �����������
#define RES_INAVLID		0
#define RES_400			1
#define RES_800			2
#define RES_1600		3
#define RES_3200		4	//������� ��������
#define RES_6400		5	//������� ��������

#define STAB_CONTROL_TRUE	1
#define STAB_CONTROL_FALSE	0
//��������� ��������� ��������� ��������� �������
typedef struct 
{
	uint32_t id_dsa_task;
	uint16_t spectrum_type; //��� �������
	uint16_t hsmf_channel; //������ ������ HSMF
	uint16_t freq_channel; //������ ������ ������� ��������
	uint16_t decimation; //���
	uint16_t envelope; //��
	uint16_t spec_res; //����������
	uint16_t averaging_num; //���������� ����������
	uint16_t stab_control; //������������
	float rot_spd_min; //����������� �������� �������� � ��
	float rot_spd_max; //������������ �������� �������� � ��
	float rscm_realization; //����������� ���������� ��������� ������� �������� �� ����������
	float rscm_measure; //����������� ���������� ��������� ������� �������� �� ���������
}spectrum_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� ������� ���������
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t hsmf_channel; //������ ������ HSMF
	uint16_t averaging_num; //���������� ����������
	uint32_t time; //����������� ��������� ���������� ��� ������, ��
	float coef_cover; //����������� ���������� ����������
}kurtosis_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� ��� ���������������� ������� (�������������)
//���������� ������ ������ ��� �������������
#define GOST_VSKZ_FROM_HZ	10.0
#define GOST_VSKZ_TO_HZ		1000.0
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t hsmf_channel; //������ ������ HSMF
	uint16_t averaging_num; //���������� ����������
	float from_hz;	//������ ������ ��
	float to_hz;	//������ ������ ��
}std_integ_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� �������������� ������ �� ���������� �������
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t hsmf_channel; //������ ������ HSMF
	uint16_t aligned_field;
	uint32_t time; //��������� ���������� ��� ���������� ������
}calc_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ��������� ��������� ��������
typedef struct
{
	uint32_t id_dsa_task;
	uint16_t hsmf_channel; //������ ������ HSMF
	uint16_t freq_channel; //������ ������ ������� ��������
	uint16_t measure_num; //���������� ���������
	uint16_t averaging_num; //���������� ����������
	float rot_spd_min; //����������� �������� �������� � ��
	float rot_spd_max; //������������ �������� �������� � ��
	uint16_t vector_num; //���������� ������� � �������� ������� (��������)
	uint16_t aligned_field; //������������� ����
}vector_frot_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ��������� ������ ���������� �������
typedef struct
{
	uint32_t id_dsa_task;
}timesignal_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ����������� ��������
typedef struct 
{
	uint16_t channel;	//����� (0,1)
	uint16_t labels; //���������� �����
	uint32_t observation_time; //����� ���������� (0 - ���� �� ����������) 
	uint32_t callback_period; //������ ����������
}freq_monitor_t;
//------------------------------------------------------------------
//��������� ��������� ����������� ����������
typedef struct 
{
	uint16_t channel; //����� (0-3)
	uint16_t subchannel; //������������� (0-7)
	uint16_t sensor; //�����
	uint16_t gain; //��� ��������
	float sensitivity; //����������������
	float manual_gain; //�������� ��� ������ ������
	float dc_min; //����������� �������� ��������� ��� �������� �������������� �������
	float dc_max; //������������ �������� ��������� ��� �������� �������������� �������
	uint32_t observation_time; //����� ���������� (0 - ���� �� ����������)
	uint32_t realization_time; //����� ����������
}stat_monitor_t;
//------------------------------------------------------------------
// ������ ������������� ��������� � �����
#define CALIBS_SIZE (4*sizeof(t_adc_values))
// ������� ������� (���-�� ��������� ������������� � ��������)
#define K1_NUM	8
#define K2_NUM	16
// ������������� ��������� ��� ������ ������
#ifndef __uClinux__
typedef struct {
	float Ucm;
	float Usd;
	float offset10;
	float K0G;
	float K1G[K1_NUM];
	float K2G[K2_NUM];
}t_adc_values;
#endif
//------------------------------------------------------------------
//��������� ����������� ������� ��������
typedef struct {
	timeval last_time;
	float last_value;
	timeval first_time;
	float mean_value;
	int32_t values_num;
}monitor_result_t;
//------------------------------------------------------------------
//��������� ����������� �������������� ������ �������
typedef struct {
	float min;
	float max;
	float mean;
	float std;
	float kurtosis;
	float peak_factor;
	float gain;
	bool overflow;
	bool underflow;
}monitor_stat_result_t;
//------------------------------------------------------------------
//��������� ������ DSA
//��������� ���������
typedef struct
{
	int32_t size;
	int32_t version;
	uint32_t id_dsa_task; //������������� ������
	uint32_t subtask_num; //����� ��������
	int32_t state; //��������� ������
	uint32_t warnings; //��������������
	int32_t error_code; //��� ������������
}dsa_header_t;
//------------------------------------------------------------------
//��������� ��������� ��������� DSA
typedef struct 
{
	int32_t size;
	int32_t version;
	uint32_t type; //��� ���������
	int32_t error_code; //��� ������������
	uint32_t warnings; //��������������
	uint32_t result_size; //������ ��������� � �����������
}dsa_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� �������
typedef struct 
{
	int32_t size;
	int32_t version;
	int32_t idx_subtask;
	float average_freq;
	float freq_step;
	float gain;
	uint16_t spectrum_type;
	uint16_t decimation;
	uint16_t envelope;
	uint16_t spec_res;
	uint16_t averaging_num;
	uint16_t sensor;
	float sensitivity;
	uint32_t sp_len;
	float* spectrum;
}result_spectrum_t;
//------------------------------------------------------------------
//��������� ��������� ���������� ��������� ������� ���������
typedef struct
{
	int32_t size;
	int32_t version;
	int32_t idx_subtask;
	float mean;
	float std;
	float gain;
	uint32_t time;
	float coef_cover;
	uint16_t averaging_num;
	uint16_t sensor;
	float sensitivity;
}result_kurtosis_t;
//------------------------------------------------------------------
//��������� ��������� ���������� ��� ���������������� ������� (�������������)
typedef struct
{
	int32_t size;
	int32_t version;
	int32_t idx_subtask;
	float std_integ; //���������
	float gain;
	float from_Hz;
	float to_Hz;
	uint16_t averaging_num;
	uint16_t sensor;
	float sensitivity;
}result_std_integ_t;
//------------------------------------------------------------------
//��������� ��������� �������������� ������ ���������� �������
typedef struct
{
	int32_t size;
	int32_t version;
	int32_t idx_subtask;
	float min;//���������
	float max;//���������
	float mean;//���������
	float std;//���������
	float kurtosis;//���������
	float peak_factor;//���������
	float gain;//���������
	uint32_t time;
	uint16_t sensor;
	uint16_t aligned_field;
	float sensitivity;
}result_calc_t;
//------------------------------------------------------------------
//��������� ���������� �������
typedef struct
{
	float ampl;
	float phase;
}vector_t;
//��������� �������������� �������� ��������
#define IS_VALID_TRUE		1
#define IS_VALID_FALSE		0
typedef struct
{
	uint16_t harm_num;
	uint16_t is_valid;
	float f_rot;
	float aver_ampl;
	float aver_phase;
	uint32_t measure_num;
	vector_t* measure;
}vector_frot_t;
//��������� ��������� ���������� �������� ��������
typedef struct 
{
	int32_t size;
	int32_t version;
	int32_t idx_subtask;
	float f_rot;//���������
	float gain;//���������
	uint16_t vector_num;//���������
	uint16_t averaging_num;//���������
	uint16_t sensor;
	float sensitivity;
	vector_frot_t* f_rot_vector;//���������
}result_vector_frot_t;
//------------------------------------------------------------------
//��������� ���������� ���������
typedef struct
{
	int32_t idx_subtask;
	uint32_t type;
	union{
		result_spectrum_t spectrum;
		result_kurtosis_t kurtosis;
		result_std_integ_t std_integ;
		result_calc_t calculation;
		result_vector_frot_t vector_frot;
	};
}result_subtask_t;
//------------------------------------------------------------------
typedef struct 
{
	uint32_t channel;
	int32_t ec;
	float gain;
	uint32_t all_samples;
	uint32_t wrote_samples;
	uint32_t wrote_labels;
}ts_progress_t;
//------------------------------------------------------------------
#define FD_DAT		1
#define FD_ROT		2
#define FD_INF		3
//------------------------------------------------------------------
//������ ������� 3, 4
typedef struct
{
	uint32_t channel;
	uint32_t type;
	uint32_t block_size;
	uint32_t cnt_block;
	uint32_t file_size;
}ts_filedesc_t;
//------------------------------------------------------------------
//������ ������� 5
typedef struct
{
	uint32_t adc_channel;
	uint32_t sub_channel;
	uint32_t freq_channel;
	float gain;
	float gain_p3;
}ts_begin_t;

typedef struct
{
	uint32_t type;
	uint32_t channel;
	uint32_t length;
	char* buffer;
}ts_buffer_t;

typedef struct  
{
	int32_t ec;
	uint32_t adc_channel;
	uint32_t sub_channel;
	uint32_t freq_channel;
	uint32_t all_samples;
	uint32_t wrote_samples;
	uint32_t wrote_labels;
	float gain;
	float gain_p3;
}ts_end_t;
//------------------------------------------------------------------
#define FLAG_SEND_TS_WRITE		0x01
//------------------------------------------------------------------
#endif

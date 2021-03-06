//------------------------------------------------------------------
#ifndef SOCKET_STRUCT_H
#define SOCKET_STRUCT_H
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
	unsigned int command;
	unsigned int length;
	char* buff;
}tcp_packet_t;
//------------------------------------------------------------------
//��������� ��������� ������ �� ������ ���������� �������
typedef struct 
{
	unsigned short channel; //����� (0-3)
	unsigned short subchannel; //����� �������������� (0-7)
	unsigned short freq_channel; //����� �� 0,1
	unsigned short sensor; //����� ������� TCP_SENSOR_
	unsigned short secs; //����� ������ � ��������
	unsigned short gsm; //��� ������ ������������ �������� TCP_GSM_XXX
	float req_gain; //����������� �������� ��� ������� ������ �������
}ts_channel_t;
//��������� ���� ������� �� ������ ���������� �������
typedef struct
{
	unsigned short begin_pause; //��������� ����� ����� �������
	unsigned short num_ch; //����� ������� ��� ������ (�� ������ 4�)
	ts_channel_t* ch; //������ �������� �������
}ts_task_t;
//------------------------------------------------------------------
//���������� ���������� (�����)
typedef struct 
{
	unsigned short id;
	unsigned short dev_lib_version;
	unsigned short interface_version;
	unsigned short server_version;
	unsigned int len_dev_name;
	char* dev_name;
}tcp_hal_desc_t;
//------------------------------------------------------------------
//��������� ��������� ������������� �������
typedef struct
{	
	unsigned short channel; //����� (0-3)
	unsigned short subchannel; //������������� (0-7)
	unsigned short sensor; //�����
	short volume; //���������
	float sensitivity; //����������������
	float dc_min; //����������� �������� ��������� ��� �������� �������������� �������
	float dc_max; //������������ �������� ��������� ��� �������� �������������� �������
}audio_task_t;
//------------------------------------------------------------------
//��������� ��������� ������ HSMF ��� ������ DSA
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short channel; //����� (0-3)
	unsigned short subchannel; //����� �������������� (0-7)
	unsigned short sensor; //����� TCP_SENSOR_XXX
	unsigned short gsm; //��� ������ ������������ �������� TCP_GSM_XXX
	float req_gain; //����������� �������� ��� ������� ������ �������
	float sensitivity; //����������������
	float dc_min; //����������� ���������
	float dc_max; //������������ ���������
}hsmf_channel_t;
//------------------------------------------------------------------
//��������� ��������� ������ ������� �������� ��� ������ DSA
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short channel; //����� �� 1,2
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
	unsigned long id_dsa_task;
	unsigned short spectrum_type; //��� �������
	unsigned short hsmf_channel; //������ ������ HSMF
	unsigned short freq_channel; //������ ������ ������� ��������
	unsigned short decimation; //���
	unsigned short envelope; //��
	unsigned short spec_res; //����������
	unsigned short averaging_num; //���������� ����������
	unsigned short stab_control; //������������
	float rot_spd_min; //����������� �������� �������� � ��
	float rot_spd_max; //������������ �������� �������� � ��
	float rscm_realization; //����������� ���������� ��������� ������� �������� �� ����������
	float rscm_measure; //����������� ���������� ��������� ������� �������� �� ���������
}spectrum_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� ������� ���������
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short hsmf_channel; //������ ������ HSMF
	unsigned short averaging_num; //���������� ����������
	unsigned long time; //����������� ��������� ���������� ��� ������, ��
	float coef_cover; //����������� ���������� ����������
}kurtosis_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� ��� ���������������� ������� (�������������)
//���������� ������ ������ ��� �������������
#define GOST_VSKZ_FROM_HZ	10.0
#define GOST_VSKZ_TO_HZ		1000.0
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short hsmf_channel; //������ ������ HSMF
	unsigned short averaging_num; //���������� ����������
	float from_hz;	//������ ������ ��
	float to_hz;	//������ ������ ��
}std_integ_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ���������� �������������� ������ �� ���������� �������
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short hsmf_channel; //������ ������ HSMF
	unsigned short aligned_field;
	unsigned long time; //��������� ���������� ��� ���������� ������
}calc_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ��������� ��������� ��������
typedef struct
{
	unsigned long id_dsa_task;
	unsigned short hsmf_channel; //������ ������ HSMF
	unsigned short freq_channel; //������ ������ ������� ��������
	unsigned short measure_num; //���������� ���������
	unsigned short averaging_num; //���������� ����������
	float rot_spd_min; //����������� �������� �������� � ��
	float rot_spd_max; //������������ �������� �������� � ��
	unsigned short vector_num; //���������� ������� � �������� ������� (��������)
	unsigned short aligned_field; //������������� ����
}vector_frot_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� ��������� ������ ���������� �������
typedef struct
{
	unsigned long id_dsa_task;
}timesignal_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ����������� ��������
typedef struct 
{
	unsigned short channel;	//����� (0,1)
	unsigned short labels; //���������� �����
	unsigned long observation_time; //����� ���������� (0 - ���� �� ����������) 
	unsigned long callback_period; //������ ����������
}freq_monitor_t;
//------------------------------------------------------------------
//��������� ��������� ����������� ����������
typedef struct 
{
	unsigned short channel; //����� (0-3)
	unsigned short subchannel; //������������� (0-7)
	unsigned short sensor; //�����
	unsigned short gain; //��� ��������
	float sensitivity; //����������������
	float manual_gain; //�������� ��� ������ ������
	float dc_min; //����������� �������� ��������� ��� �������� �������������� �������
	float dc_max; //������������ �������� ��������� ��� �������� �������������� �������
	unsigned long observation_time; //����� ���������� (0 - ���� �� ����������)
	unsigned long realization_time; //����� ����������
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
	int values_num;
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
	int size;
	int version;
	unsigned long id_dsa_task; //������������� ������
	unsigned int subtask_num; //����� ��������
	int state; //��������� ������
	unsigned long warnings; //��������������
	int error_code; //��� ������������
}dsa_header_t;
//------------------------------------------------------------------
//��������� ��������� ��������� DSA
typedef struct 
{
	int size;
	int version;
	unsigned int type; //��� ���������
	int error_code; //��� ������������
	unsigned long warnings; //��������������
	unsigned int result_size; //������ ��������� � �����������
}dsa_subtask_t;
//------------------------------------------------------------------
//��������� ��������� ��������� �������
typedef struct 
{
	int size;
	int version;
	int idx_subtask;
	float average_freq;
	float freq_step;
	float gain;
	unsigned short spectrum_type;
	unsigned short decimation;
	unsigned short envelope;
	unsigned short spec_res;
	unsigned short averaging_num;
	unsigned short sensor;
	float sensitivity;
	unsigned int sp_len;
	float* spectrum;
}result_spectrum_t;
//------------------------------------------------------------------
//��������� ��������� ���������� ��������� ������� ���������
typedef struct
{
	int size;
	int version;
	int idx_subtask;
	float mean;
	float std;
	float gain;
	unsigned long time;
	float coef_cover;
	unsigned short averaging_num;
	unsigned short sensor;
	float sensitivity;
}result_kurtosis_t;
//------------------------------------------------------------------
//��������� ��������� ���������� ��� ���������������� ������� (�������������)
typedef struct
{
	int size;
	int version;
	int idx_subtask;
	float std_integ; //���������
	float gain;
	float from_Hz;
	float to_Hz;
	unsigned short averaging_num;
	unsigned short sensor;
	float sensitivity;
}result_std_integ_t;
//------------------------------------------------------------------
//��������� ��������� �������������� ������ ���������� �������
typedef struct
{
	int size;
	int version;
	int idx_subtask;
	float min;//���������
	float max;//���������
	float mean;//���������
	float std;//���������
	float kurtosis;//���������
	float peak_factor;//���������
	float gain;//���������
	unsigned long time;
	unsigned short sensor;
	unsigned short aligned_field;
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
	unsigned short harm_num;
	unsigned short is_valid;
	float f_rot;
	float aver_ampl;
	float aver_phase;
	unsigned int measure_num;
	vector_t* measure;
}vector_frot_t;
//��������� ��������� ���������� �������� ��������
typedef struct 
{
	int size;
	int version;
	int idx_subtask;
	float f_rot;//���������
	float gain;//���������
	unsigned short vector_num;//���������
	unsigned short averaging_num;//���������
	unsigned short sensor;
	float sensitivity;
	vector_frot_t* f_rot_vector;//���������
}result_vector_frot_t;
//------------------------------------------------------------------
//��������� ���������� ���������
typedef struct
{
	int idx_subtask;
	unsigned int type;
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
	unsigned int channel;
	int ec;
	float gain;
	unsigned int all_samples;
	unsigned int wrote_samples;
	unsigned int wrote_labels;
}ts_progress_t;
//------------------------------------------------------------------
#define FD_DAT		1
#define FD_ROT		2
#define FD_INF		3
//------------------------------------------------------------------
//������ ������� 3, 4
typedef struct
{
	unsigned int channel;
	unsigned int type;
	unsigned int block_size;
	unsigned int cnt_block;
	unsigned long file_size;
}ts_filedesc_t;
//------------------------------------------------------------------
//������ ������� 5
typedef struct
{
	unsigned int adc_channel;
	unsigned int sub_channel;
	unsigned int freq_channel;
	float gain;
	float gain_p3;
}ts_begin_t;

typedef struct
{
	unsigned int type;
	unsigned int channel;
	unsigned int length;
	char* buffer;
}ts_buffer_t;

typedef struct  
{
	int ec;
	unsigned int adc_channel;
	unsigned int sub_channel;
	unsigned int freq_channel;
	unsigned int all_samples;
	unsigned int wrote_samples;
	unsigned int wrote_labels;
	float gain;
	float gain_p3;
}ts_end_t;
//------------------------------------------------------------------
#define FLAG_SEND_TS_WRITE		0x01
//------------------------------------------------------------------
#endif

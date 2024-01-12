#ifndef ZLG_CONFIG_INTF_H
#define ZLG_CONFIG_INTF_H

struct _Meta;
struct _Pair;
struct _Options;
struct _ConfigNode;

typedef struct _Meta Meta;
typedef struct _Pair Pair;
typedef struct _Options Options;
typedef struct _ConfigNode ConfigNode;

/**
 *  \struct Options
 * �ڵ�mata�Ŀ�ѡ�
 */
struct _Options
{
	/*! ��ѡ�����������*/
	const char * type;

	/*! ��ѡ���ֵ*/
	const char * value;

	/*! ��ѡ���������Ϣ*/
	const char * desc;
};

/**
 *  \struct Meta
 * �ڵ�mata��Ϣ��
 */
struct _Meta
{
	/*! ��������������� */
	const char * type;

	/*! �������˵������Ϣ */
	const char * desc;

	/*! �������Ƿ���ֻ���ģ�ȱʡΪ�ɶ�д */
	int read_only;

	/*! �����������ʽ����ʾ */
	const char * format;

	/*! ������ֵ���͵���������˵����Сֵ�����ַ�������������˵����С���ȣ��ֽ������� */
	double min_value;

	/*! ������ֵ���͵���������˵�����ֵ�����ַ�������������˵����󳤶ȣ��ֽ������� */
	double max_value;

	/*! ������ĵ�λ */
	const char * unit;

	/*! ͨ����ť/���ֵȷ�ʽ�޸�������ʱ������ */
	double delta;

    /*! �������Ƿ�ɼ�, true�ɼ���false���ɼ���Ҳ���԰󶨱��ʽ�����ʽʹ�òο�demo3����ȱʡ�ɼ� */
    const char* visible;
    
    /*! ���������Ƿ�ʹ��, trueʹ�ܣ�false��ʹ�ܣ�Ҳ���԰󶨱��ʽ�����ʽʹ�òο�demo3����ȱʡʹ�� */
    const char* enable;

	/*! ������Ŀ�ѡֵ��������type��Ϊ�������ʱ��Ч */
	int editable;

	/*! ������Ŀ�ѡֵ��������type��Ϊ�������ʱ��Ч����NULL���� */
	Options** options;
};

/**
 *  \struct Pair
 *  ���Ե�KeyValue�ԡ�
 */
struct _Pair
{
	const char * key;
	const char * value;
};

/**
 *  \struct ConfigNode 
 *  ConfigNode
 */
struct _ConfigNode
{
	/*! �ڵ������ */
	const char * name;
	/*! �ڵ��ֵ ͬ�����԰󶨱��ʽ*/
	const char * value;
    /*! �ڵ�ֵ�ı��ʽ�����иñ��ʽʱ��value�ɴ˱��ʽ�������*/
    const char* binding_value;
	/*! �ýڵ��·�� */
	const char * path;
	/*! ��������Ϣ */
	Meta* meta_info;
	/*! �ýڵ���ӽڵ�, ��NULL����*/
	ConfigNode** children;
	/*! �ýڵ������, ��NULL����*/
	Pair** attributes;
};

/**
 * \brief ��ȡ���Ե�������Ϣ��
 *
 * \retval ConfigNode
 */
typedef const ConfigNode* (*GetPropertysFunc)();

/**
 * \brief ����ָ��·�������Ե�ֵ��
 * \param[in] path  : ���Ե�·����
 * \param[in] value : ���Ե�ֵ��
 *
 * \retval �ɹ�����1��ʧ�ܷ���0��
 */
typedef int (*SetValueFunc)(const char* path, const char* value);

/**
 * \brief ��ȡָ��·�������Ե�ֵ��
 * \param[in] path  : ���Ե�·����
 * \retval �ɹ��������Ե�ֵ��ʧ�ܷ���NULL��
 */
typedef const char* (*GetValueFunc)(const char* path);

typedef struct  tagIProperty
{
	SetValueFunc     SetValue;
	GetValueFunc     GetValue;
	GetPropertysFunc GetPropertys;
}IProperty;

#endif/*ZLG_CONFIG_INTF_H*/


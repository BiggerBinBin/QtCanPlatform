#ifndef _Z_DBC_H_
#define _Z_DBC_H_

#ifdef WIN32
    #define FUNCCALL __stdcall
#else
    #define FUNCCALL
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

typedef unsigned int     uint32;
typedef unsigned int     DBCHandle;
typedef unsigned char    uint8;
typedef unsigned short   uint16;
typedef unsigned long long uint64;

// error code
typedef uint8 ERR_CODE;
#define ERR_SUCCESS            0
#define ERR_FAILED             1
#define ERR_MULTI_TRANSMITTING 2                //多帧传输发送中
#define ERR_NOT_SUPPORT        3                //功能不支持, 或未开启

#define INVALID_DBC_HANDLE        -1            // 无效的DBC句柄

#define _MAX_FILE_PATH_           260           // 最长文件路径
#define _DBC_NAME_LENGTH_         127           // 名称最长长度
#define _DBC_COMMENT_MAX_LENGTH_  200           // 注释最长长度
#define _DBC_UNIT_MAX_LENGTH_     10            // 单位最长长度
#define _DBC_SIGNAL_MAX_COUNT_    256           // 一个消息含有的信号的最大数目

// multiplexer type
#define MUTIPLEXER_NONE     0                   // 不使用复用器
#define MUTIPLEXER_M_VALUE  1                   // 复用信号，当复用器开关的值为multiplexer_value时，该信号有效
#define MUTIPLEXER_M_SWITCH 2                   // 复用器开关，一个DBC消息只能有一个信号为开关

// CAN type
#define FT_CAN   0                              //CAN帧
#define FT_CANFD 1                              //CANFD帧

struct DBCSignal
{
    uint32 nStartBit;                               // 起始位
    uint32 nLen;                                    // 位长度
    double nFactor;                                 // 转换因子
    double nOffset;                                 // 转换偏移 实际值=原始值*nFactor+nOffset
    double nMin;                                    // 最小值
    double nMax;                                    // 最大值
    //double nValue;                                  // 实际值, 不再使用，改为通过 ZDBC_CalcActualValue 计算
    uint64 nRawValue;                               // 原始值
    bool is_signed;                                 // 是否有符号数据
    bool is_motorola;                               // 是否摩托罗拉格式
    uint8 multiplexer_type;                         // 复用器类型，see 'multiplexer type' above
    uint8 val_type;                                 // 0:integer, 1:float, 2:double
    uint32 multiplexer_value;                       // 复用器开关值为此值时信号有效
    char unit[_DBC_UNIT_MAX_LENGTH_+1];             // 单位
    char strName[_DBC_NAME_LENGTH_+1];              // 名称
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];    // 注释
    char strValDesc[_DBC_NAME_LENGTH_+1];           // 值描述
};

struct DBCMessage
{
    uint32 nSignalCount;                            // 信号数量
    uint32 nID;                                     // 消息ID
    uint32 nSize;                                   // 消息占的字节数目
    double nCycleTime;                              // 发送周期
    uint8 nExtend;                                  // 1:扩展帧, 0:标准帧
    DBCSignal vSignals[_DBC_SIGNAL_MAX_COUNT_];     // 信号集合
    char strName[_DBC_NAME_LENGTH_+1];              // 名称
    char strComment[_DBC_COMMENT_MAX_LENGTH_+1];    // 注释
};

struct DBCNetworkNode
{
    char strName[_DBC_NAME_LENGTH_+1];              // 网络节点名称
    uint32 stationAddr;                             // 地址
};

// 数据帧结构类型
#define ZDBC_FRAME_TYPE_ZLGCAN      0               // 使用zlgcan库的can_frame 或者 canfd_frame       
#define ZDBC_FRAME_TYPE_CONTROLCAN  1               // 使用ControlCAN的VCI_CAN_OBJ

// DBC协议类型
#define PROTOCOL_J1939              0               // J1939协议
#define PROTOCOL_OTHER              1               // 其他协议, 如果DBC文件中未指定，默认为此协议

struct FileInfo
{
    char strFilePath[_MAX_FILE_PATH_+1]; // dbc文件路径，中文路径请使用GB2312编码
    uint8 merge;                         // 1:不清除现有的数据, 即支持加载多个文件;0：清除原来的数据
};

//值与含义对, 例如3 "Not Supported" 2 "Error" 1 "Not Defined"
struct ValDescPair
{
    double value;
    char desc[_DBC_NAME_LENGTH_+1];
};

/*
功能说明：库没实际发送数据功能, 只提供发送的数据,
          调用者需要实现该回调函数处理实际数据发送。
参    数[in]：ctx-回调函数的上下文
参    数[in]：pObj-要发送的数据
返 回 值：true发送成功
*/
typedef bool (FUNCCALL *OnSend)(void* ctx, void* pObj);

/*
功能说明：发送CANFD帧的回调
参    数[in]：ctx-回调函数的上下文
参    数[in]：pObj-要发送的数据
返 回 值：true发送成功
*/
typedef bool (FUNCCALL *OnSendCANFD)(void* ctx, void* pObj);

/*
功能说明：多帧传输完成后回调。也是ZDBC_Analyse多帧消息解析完成后的回调。
参    数[in]：ctx-回调函数的上下文
参    数[in]：pMsg-返回的DBC消息
参    数[in]：data-多帧的实际数据
参    数[in]：nLen-多帧数据的字节数目
参    数[in]：nDirection-多帧的传输方向 1:接收, 0:发送
返 回 值：void
*/
typedef void (FUNCCALL *OnMultiTransDone)(void* ctx, DBCMessage* pMsg, uint8* data, uint16 nLen, uint8 nDirection);

/*
功能说明：ZDBC_AsyncAnalyse 异步解析完成后的回调
参    数[in]：ctx-回调函数的上下文
参    数[in]：pMsg-返回的DBC消息
参    数[in]：extraData-请求解析时传入的标识
*/
typedef void (FUNCCALL *OnAsyncAnalyseDone)(void* ctx, DBCMessage* pMsg, uint64 extraData);

/*
功能说明：设置内部日志输出文件, 默认无日志输出
参    数[in]：enable， 1-开启日志输出, 0-关闭日志输出
参    数[in]：filepath 日志文件路径, enable为0时可传NULL
*/
EXTERN_C  void FUNCCALL ZDBC_SetLogFile( uint8 enable, const char* filepath );

/*
功能说明：设置数据帧结构体类型，默认为ZDBC_FRAME_TYPE_ZLGCAN
参    数[in]：type 数据帧结构类型，ZDBC_FRAME_TYPE_ZLGCAN 或 ZDBC_FRAME_TYPE_CONTROLCAN
*/
EXTERN_C  void   FUNCCALL ZDBC_SetFrameType( uint32 type );
EXTERN_C  uint32 FUNCCALL ZDBC_GetFrameType();

/*
功能说明：初始化解析模块。
参    数[in]：disableMultiSend-是否关闭多帧发送； 0-不关闭，独立线程处理多帧发送交互; 1-关闭，不支持多帧的消息发送
参    数[in]：enableAsyncAnalyse-是否开启异步解析； 0-不启动，ZDBC_AsyncAnalyse接口无效；1-启动, 独立线程解析出消息
返 回 值：INVALID_DBC_HANDLE:初始化失败
          其他值:初始化成功
          保存该值作后续使用
*/
EXTERN_C  DBCHandle FUNCCALL ZDBC_Init(uint8 disableMultiSend = 0, uint8 enableAsyncAnalyse = 0);

/*
功能说明：释放资源, 与DBC_Init配对使用
参    数[in]：hDBC-句柄, DBC_Load的返回值
*/
EXTERN_C  void FUNCCALL ZDBC_Release( DBCHandle hDBC );

/*
功能说明：加载DBC格式文件。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：pFileInfo-文件信息, 参考struct FileInfo
返 回 值：true加载成功
*/
EXTERN_C  bool FUNCCALL ZDBC_LoadFile( DBCHandle hDBC, const FileInfo* pFileInfo );

/**
* \brief 设置dbc文件 协议
* \param[in]   type   : dbc的协议类型, j1939选择PROTOCOL_J1939, 其他协议选择PROTOCOL_OTHER
* \retval 成功返回true，失败返回false。
*/
EXTERN_C void FUNCCALL ZDBC_SetProtocol( DBCHandle hDBC, uint8 type);

/**
* \brief 返回加载的dbc文件的协议类型, PROTOCOL_J1939, PROTOCOL_OTHER
*/
EXTERN_C uint8 FUNCCALL ZDBC_GetProtocol( DBCHandle hDBC );

/*
功能说明：获取文件的第一条消息。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[out]：pMsg 存储消息的信息
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_GetFirstMessage( DBCHandle hDBC, DBCMessage* pMsg );

/*
功能说明：获取下一条消息。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[out]：pMsg 存储消息的信息
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_GetNextMessage( DBCHandle hDBC, DBCMessage* pMsg );

/*
功能说明：根据ID获取消息。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：nID-消息的ID
参    数[out]：pMsg 存储消息的信息
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_GetMessageById( DBCHandle hDBC, uint32 nID, DBCMessage* pMsg );

/*
功能说明：获取文件消息个数。
参    数[in]：hDBC-句柄, DBC_Load的返回值
返 回 值：消息个数
*/
EXTERN_C  uint32 FUNCCALL ZDBC_GetMessageCount( DBCHandle hDBC );

/*
功能说明：根据DBC协议解析帧数据
         如果为多帧消息，多次传入各帧数据，通过OnMultiTransDone回调传出解析结果
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：pObj 原始帧数据, ControlCAN传入VCI_CAN_OBJ, zlgcan传入can_frame 或者 canfd_frame
参    数[in]：frame_type 帧类型, 参考FT_CAN、FT_CANFD，ControlCAN不支持CANFD
参    数[out]：pMsg 解析结果
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_Analyse( DBCHandle hDBC, const void* pObj, uint8 frame_type, DBCMessage* pMsg );

/*
功能说明：根据DBC协议解析帧数据为DBCMessage, 异步解析，独立线程; 使用此接口需要ZDBC_Init传入参数enableAsyncAnalyse=1。
         如果为多帧消息，多次传入各帧数据。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：pObj 原始帧数据, ControlCAN传入VCI_CAN_OBJ, zlgcan传入can_frame 或者 canfd_frame
参    数[in]：frame_type 帧类型, 参考FT_CAN、FT_CANFD，ControlCAN不支持CANFD
参    数[in]：extraData 此帧数据的附带数据，解析完成后会回调传回此参数；例如帧时间戳
返 回 值：true表示进入解析队列成功, 通过 ZDBC_SetAsyncAnalyseCallback 设置的回调传出解析结果
*/
EXTERN_C  bool FUNCCALL ZDBC_AsyncAnalyse(DBCHandle hDBC, const void* pObj, uint8 frame_type, uint64 extraData);

/*
功能说明：设置异步解析回调函数, 单帧消息或多帧消息都使用此回调。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：cb 回调函数
参    数[in]：ctx 上下文
*/
EXTERN_C  void FUNCCALL ZDBC_SetAsyncAnalyseCallback( DBCHandle hDBC, OnAsyncAnalyseDone cb, void* ctx);

/*
功能说明：调用者需要调用该函数把接收到的帧数据传进来,
         涉及多帧传输必须要这样调用, 否则无法实现设备交互。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：pObj 原始帧数据
参    数[in]：frame_type 帧类型 参考FT_CAN FT_CANFD
返 回 值：void
*/
EXTERN_C  void FUNCCALL ZDBC_OnReceive( DBCHandle hDBC, const void* pObj, uint8 frame_type );

/*
功能说明：设置实际发送数据的处理者, 涉及数据发送时必须设置，只需要设置一次。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：sender回调函数指针
参    数[in]：ctx-回调函数的上下文
返 回 值：void
*/
EXTERN_C  void FUNCCALL ZDBC_SetSender( DBCHandle hDBC, OnSend sender, void* ctx );
EXTERN_C  void FUNCCALL ZDBC_SetCANFDSender( DBCHandle hDBC, OnSendCANFD sender, void* ctx );

/*
功能说明：设置处理多帧传输数据的回调函数，只需要设置一次
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：OnMultiTransDone回调函数指针
参    数[in]：ctx-回调函数的上下文
返 回 值：void
*/
EXTERN_C  void FUNCCALL ZDBC_SetOnMultiTransDoneFunc( DBCHandle hDBC, OnMultiTransDone func, void* ctx);
/*
功能说明：发送DBC消息。
         zlgcan库可支持CANFD单帧发送，多帧时只用CAN帧发送。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：pMsg 要发送的消息
参    数[in]：frame_type 使用CAN或者CANFD帧发送，参考FT_CAN、FT_CANFD
返 回 值：参考//error code
*/
EXTERN_C  ERR_CODE FUNCCALL ZDBC_Send( DBCHandle hDBC, const DBCMessage* pMsg, uint8 frame_type  );

/*
功能说明：获取具体信号的值与含义对个数。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：msg_id, 消息ID
参    数[in]：signal_name, 信号名
返 回 值：值与含义对个数
*/
EXTERN_C  uint32 FUNCCALL ZDBC_GetValDescPairCount( DBCHandle hDBC, uint32 msg_id, const char* signal_name);

/*
功能说明：获取值与含义对。
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：msg_id, 消息ID
参    数[in]：signal_name, 信号名
参    数[in,out]：pair 数组,调用者构造,大小为DBC_GetValDescPairCount返回值
返 回 值：true表示成功
*/
EXTERN_C  void FUNCCALL ZDBC_GetValDescPair( DBCHandle hDBC, uint32 msg_id, const char* signal_name, ValDescPair* pair);

/*
功能说明：根据原始数据解码为DBCMessage
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[out]：pMsg 解析结果
参    数[in]：pObj 帧数据数组, ControlCAN传入VCI_CAN_OBJ, zlgcan传入can_frame
参    数[in]：nCount 原始帧数据个数, 即数组大小
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_Decode( DBCHandle hDBC, DBCMessage* pMsg, const void* pObj, uint32 nCount );

/*
功能说明：根据DBCMessage编码为原始数据
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[out]：pObj 编码的原始数据缓冲区数组, ControlCAN传入VCI_CAN_OBJ, zlgcan传入can_frame
参    数[in,out]：pObj 缓冲区大小, 返回时为实际原始数据个数
参    数[in]：pMsg 要编码的消息
返 回 值：true表示成功
*/
EXTERN_C  bool FUNCCALL ZDBC_Encode( DBCHandle hDBC, void* pObj, uint32* nCount, const DBCMessage* pMsg );

/*
功能说明：信号原始值转换为实际值
参    数[in]：sgl 信号
参    数[in,out]：rawVal 原始值, 如果该值超出信号长度可表示范围，会被截断。
返 回 值：实际值
*/
EXTERN_C double FUNCCALL ZDBC_CalcActualValue(const DBCSignal* sgl, uint64 *rawVal);

/*
功能说明：信号实际值转换为原始值
参    数[in]：sgl 信号
参    数[in,out]：actualVal 实际值, 超出可表示范围时会被修正
返 回 值：原始值
*/
EXTERN_C uint64 FUNCCALL ZDBC_CalcRawValue(const DBCSignal* sgl, double *actualVal);

/*
功能说明：获取网络节点数量
参    数[in]：hDBC-句柄, DBC_Load的返回值
返 回 值：网络节点总数量
*/
EXTERN_C uint32 FUNCCALL ZDBC_GetNetworkNodeCount(DBCHandle hDBC);

/*
功能说明：获取指定索引的网络节点信息
参    数[in]：hDBC-句柄, DBC_Load的返回值
参    数[in]：index 位置索引
参    数[out]：node 网络节点信息
返 回 值：成功或失败
*/
EXTERN_C bool FUNCCALL ZDBC_GetNetworkNodeAt(DBCHandle hDBC, uint32 index, DBCNetworkNode *node);


#endif 

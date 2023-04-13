#ifndef FUN1_HTTPServer_h
#define FUN1_HTTPServer_h

#include "TypeMro.h"
#define HttpURLMaxLength      100  //URL最大字节数，含\0

/*
响应码分五种类型，由它们的第一位数字表示：
1xx：信息，请求收到，继续处理
2xx：成功，行为被成功地接受、理解和采纳
3xx：重定向，为了完成请求，必须进一步执行的动作
4xx：客户端错误，请求包含语法错误或者请求无法实现
5xx：服务器错误，服务器不能实现一种明显无效的请求

下表显示每个响应码及其含义：
100 继续101 分组交换协
200 OK  201 被创建 202 被采纳 203 非授权信息 204 无内容 205 重置内容 206 部分内容
300 多选项 301 永久地传送  302 找到3 03 参见其他 304 未改动 305 使用代理 307 暂时重定向
400 错误请求 401 未授权 402 要求付费 403 禁止 404 未找到 405 不允许的方法 406 不被采纳 407 要求代理授权 408 请求超时 409  冲突 410 过期的 411 要求的长度 412 前提不成立 413 请求实例太大 414 请求URI太大 415 不支持的媒体类型 416 无法满足的请求范围 417 失败的预期
500 内部服务器错误  501 未被使用  502 网关错误  503 不可用的服务  504 网关超时  505 HTTP版本未被支持
*/

/*
get传送的数据量较小，不能大于2KB。
post传送的数据量较大，一般被默认为不受限制。但理论上，IIS4中最大量为80KB，IIS5中为100KB。
*/

// typedef enum
// {
//      OprResult_OprErr = -1,   //操作失败
//      OprResult_OprReset = 0,  //复位
//      OprResult_DecodeOprOK,   //解码操作完成
//      OprResult_EncodeOprOK,   //编码操作完成
//      OprResult_OprIng         //操作进行中
// }Enum_OprResult;
//OprResult宏定义
#define OprResult_OprErr                -1   //操作失败
#define OprResult_OprReset              0    //复位
#define OprResult_DecodeOprIng          1    //操作进行中
#define OprResult_DecodeOprOK           2    //解码操作完成
#define OprResult_EncodeOprIng          3    //操作进行中
#define OprResult_EncodeOprOK           4    //编码操作完成

//版本宏定义
#define HttpVersion_unknow              0    //版本未知
#define HttpVersion_1d0                 1    //V1.0版本
#define HttpVersion_1d1                 2    //V1.1版本                 
//响应体内容类型宏定义----输入出
#define HttpContentType_unknow          0    //未知
#define HttpContentType_TextHtml        1    //Text/Html//GET输入
#define HttpContentType_TextCss         2    //Text/Css//GET输入
#define HttpContentType_TextJavaScript  3    //text/javascript//GET输入
#define HttpContentType_Image           4    //Image/*//GET输入
#define HttpContentType_urlencoded      7    //application/x-www-form-urlencoded//POST输出
#define HttpContentType_json            8    //application/json//POST输出
#define HttpContentType_multipart       9    //multipart/form-data//POST输出
//
// typedef enum
// {
//      Modth_Unknow = 0,   //方法未知
//      Modth_GET,          //GET方法
//      Modth_POST          //POST方法
// }Enum_Modth;
#define Modth_Unknow                    0//方法未知
#define Modth_GET                       1//GET方法
#define Modth_POST                      2//POST方法


typedef struct
{
     s8 OprResult;       //模块操作及结果反馈
//---------- 以下解码部分，参数为只读属性，用户禁止修改 -----------------
     u8 Modth;           //方法(0:未知 1:Get 2:Post)
     u8 Url[HttpURLMaxLength]; //存放URL
     u8 Version;         //http版本号(值参考版本宏定义)
     u8 Connect;         //连接状态(0:未知 1:短连接 2:长连接)     
     u8 QContentType;     //请求体类型(值参考响应体内容类型宏定义)     
     u32 QContentLength;  //请求体字节数

//---------- 以下编码部分，参数为可写属性 ----------------------------
     u16 AckCode;        // 响应码(200,404)
     u8 *AckCodeScriptSP;//响应简述字符串指针
     u8 AContentType;     //响应体类型(值参考响应体内容类型宏定义)  
     s32 AContentLength;  //响应体字节数(小于0为chunked模式)
     
//----------以下参数是模块内部使用，用户禁止访问和改动-------------------
     u8 StateMachine;    //编解码操作状态机
     u8 EncodeStateMachine;//请求头编码状态机
     MatchOPRType motv;  //请求头操作变量

}HttpServerOPRType;

typedef struct
{
     //MatchTableType *mttp;//请求头表
     HttpServerOPRType *hotp;
}HttpServerCNFType;

extern s8 FUN1_HTTPServer_Config(HttpServerCNFType *hsctp);
extern s8 FUN1_HTTPServer_Decode(HttpServerOPRType *hotp,u8 *dp,u16 *dl);
extern s8 FUN1_HTTPServer_Encode(HttpServerOPRType *hotp,u8 *dp,u16 *dl);
extern s8 FUN1_HTTPServer_ChunkedEncode(u8 *dp,u16 dl,u8 *sp,u16 *sl);

#endif

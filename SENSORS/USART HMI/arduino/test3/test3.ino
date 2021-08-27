//根据自己的实际改为对应的串口
#define TJC Serial3
#define FRAME_LENGTH 6


//发送结束符
void SendEnd() {
  TJC.write(0xff);
  TJC.write(0xff);
  TJC.write(0xff);
}


unsigned long nowtime;
void setup() {
  // put your setup code here, to run once:
  TJC.begin(115200);
  nowtime = millis(); //获取当前已经运行的时间
}


int a = 0;
void loop() {
  // put your main code here, to run repeatedly:
  char str[100];
  //实现每隔50S发送1次
  if (millis() >= nowtime + 50000)
  {
    nowtime = millis(); //获取当前已经运行的时间
    
    //用sprintf来格式化字符串，给n0的val属性赋值
    sprintf(str, "n0.val=%d", a);
    //把字符串发送出去
    TJC.print(str);
    //发送结束符
    SendEnd();

    //用sprintf来格式化字符串，给t0的txt属性赋值
    sprintf(str, "t0.txt=\"现在是%d\"", a);
    //把字符串发送出去
    TJC.print(str);
    //发送结束符
    SendEnd();

    //用sprintf来格式化字符串，触发b0的按下事件
    sprintf(str, "click b0,1");
    //把字符串发送出去
    TJC.print(str);
    //发送结束符
    SendEnd();

    //用sprintf来格式化字符串，触发b0的弹起事件    
    sprintf(str, "click b0,0");
    //把字符串发送出去    
    TJC.print(str);
    //发送结束符    
    SendEnd();

    a++;
  }


  //串口数据格式：
  //串口数据帧长度：6字节
  //包头      led编号  LED状态    包尾
  //0x55      1字节    1字节     0xffffff
  //例子1：上位机代码  printh 55 01 00 ff ff ff  含义：1号led关闭
  //例子2：上位机代码  printh 55 04 01 ff ff ff  含义：4号led打开
  //例子3：上位机代码  printh 55 00 01 ff ff ff  含义：0号led打开
  //例子4：上位机代码  printh 55 09 00 ff ff ff  含义：4号led关闭
 
  //当串口缓冲区大于等于6时
  if (TJC.available() >= FRAME_LENGTH) {
    unsigned char ubuffer[FRAME_LENGTH];
    //从缓冲区读取6个字节直到遇到0xffffff
    int ret = TJC.readBytesUntil(0xffffff, ubuffer, FRAME_LENGTH);
    //当读取到的是6个字节时
    if (ret == FRAME_LENGTH) {
      //判断ubuffer[0]是否为0x55、帧尾是否为0xffffff
      if (ubuffer[0] == 0x55 && ubuffer[3] == 0xff && ubuffer[4] == 0xff && ubuffer[5] == 0xff) {
        char str2[100];
        sprintf(str2, "t0.txt=\"led %d is %s\"", ubuffer[1], ubuffer[2]?"on":"off");
        TJC.print(str2);
        SendEnd();
      }
    }
  }

}

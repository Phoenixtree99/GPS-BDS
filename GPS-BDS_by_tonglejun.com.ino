#include <U8g2lib.h>
U8G2_ST7567_JLX12864_1_4W_HW_SPI u8g2(U8G2_R0,/* cs=*/ A1, /* dc=*/ A2, /* reset=*/ A0); 
struct
{
  char GPS_Buffer[80];
  bool isGetData;   //是否获取到GPS数据
  bool isParseData; //是否解析完成
  bool isUsefull;   //定位信息是否有效
} Save_Data;

const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;
int button_a;
int button_b;
int button_m;
int button_n;

void setup()  //初始化内容
{
  Serial.begin(9600); 
  Serial.println("Wating...");
  u8g2.begin();
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  button_a = 0;
  button_b = 0;
  button_m = 0;
  button_n = 0;
  Save_Data.isGetData = false;
  Save_Data.isParseData = false;
  Save_Data.isUsefull = false;
}

void loop()   //主循环
{
  gpsRead();  //获取GPS数据
  parseGpsBuffer();//解析GPS数据
  printGpsBuffer();//输出解析后的数据
  background_led();//背光控制
}

void errorLog(int num)  //报错
{
  Serial.print("ERROR");
  Serial.println(num);
}

void printGpsBuffer(){
  Serial.print(button_m);
  Serial.println(button_n);
  if(((button_a == 0 && button_b == 0)||(button_a == -1 && button_b == -1)) && (digitalRead(7) == LOW)){
    //page1，显示页面1
    button_a = 0;
    button_b = 0;
    if (Save_Data.isParseData)
  {
    Save_Data.isParseData = false;
    
    Serial.print("Save_Data.UTCTime = ");
    Serial.println(RMC_time(Save_Data.GPS_Buffer));
    Serial.print("Save_Data.Date = ");
    Serial.println(RMC_date(Save_Data.GPS_Buffer));
    u8g2.firstPage();
        do{
            u8g2.clearBuffer();
            draw_time();
            u8g2.sendBuffer();
          }
        while(u8g2.nextPage());
    }
  }else if( ((button_a == 0 && button_b == 0)||(button_a == 1 && button_b == 0)) && digitalRead(7) == HIGH){
    //page2，显示页面2
     if(Save_Data.isParseData)
    {
      Save_Data.isParseData = false;
      u8g2.firstPage();
        do{
            u8g2.clearBuffer();
            draw_Detail();
            u8g2.sendBuffer();
          }
        while(u8g2.nextPage());
    }
    button_a = 1;
  }else if(((button_a == 1 && button_b == 0) || (button_a == 1 && button_b ==1)) && digitalRead(7) == LOW ){
    //page2，显示页面2
    if(Save_Data.isParseData)
    {
      Save_Data.isParseData = false;
      u8g2.firstPage();
        do{
            u8g2.clearBuffer();
            draw_Detail();
            u8g2.sendBuffer();
          }
        while(u8g2.nextPage());
    }  
    button_b = 1;
  }else if( ((button_a == 1 && button_b == 1) || (button_a == -1 && button_b == -1)) && digitalRead(7) == HIGH){
    button_a = -1;
    button_b = -1;
    //page1，显示页面1
    if (Save_Data.isParseData)
   {
    Save_Data.isParseData = false;
    u8g2.firstPage();
        do{
            u8g2.clearBuffer();
            draw_time();
            u8g2.sendBuffer();
          }
        while(u8g2.nextPage());      
    }    
  }
}

void parseGpsBuffer()
{
  char *subString;
  char *subStringNext;
  if (Save_Data.isGetData)
  {
    Save_Data.isGetData = false;
    for (int i = 0 ; i <= 2 ; i++)
    {
      if (i == 0)
      {
        if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
          errorLog(1);  //解析错误
      }
      else
      {
        subString++;
        if ((subStringNext = strstr(subString, ",")) != NULL)
        {
          char usefullBuffer[2]; 
          switch(i)
          {
            case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break; //获取UTC时间
            default:break;
          }
          subString = subStringNext;
          Save_Data.isParseData = true;
          if(usefullBuffer[0] == 'A')
            Save_Data.isUsefull = true;
          else if(usefullBuffer[0] == 'V')
            Save_Data.isUsefull = false;
        }
        else
        {
          errorLog(2);  //解析错误
        }
      }
    }
  }
}


void gpsRead() {
  while (Serial.available())
  {
    gpsRxBuffer[ii++] = Serial.read();
    if (ii == gpsRxBufferLength)clrGpsRxBuffer();
  }

  char* GPS_BufferHead;
  char* GPS_BufferTail;
  if ((GPS_BufferHead = strstr(gpsRxBuffer, "$GPRMC,")) != NULL || (GPS_BufferHead = strstr(gpsRxBuffer, "$GNRMC,")) != NULL )
  {
    if (((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL) && (GPS_BufferTail > GPS_BufferHead))
    {
      memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);
      Save_Data.isGetData = true;

      clrGpsRxBuffer();
    }
  }
}

String RMC_time(String RMC_String){
  String RMC_UTCtime = "";
  String RMC_UTCtime_output = "";
  int j = 0; 
  for(int i=0;i<2;i++){
    RMC_UTCtime = "";
    j++;
    while(RMC_String[j] != ','){
      RMC_UTCtime += (char)RMC_String[j];
      j++;
    }
  }
      RMC_UTCtime_output += RMC_UTCtime[0];
      RMC_UTCtime_output += RMC_UTCtime[1];
      RMC_UTCtime_output += ':';
      RMC_UTCtime_output += RMC_UTCtime[2];
      RMC_UTCtime_output += RMC_UTCtime[3];
      RMC_UTCtime_output += ':';
      RMC_UTCtime_output += RMC_UTCtime[4];
      RMC_UTCtime_output += RMC_UTCtime[5];
      return (RMC_UTCtime_output);
}

String RMC_date(String RMC_String){
  String RMC_Date = "";
  String RMC_Date_output = "";
  int m = 0;
  for(int n=0;n<10;n++){
    RMC_Date = "";
    m++;
    while(RMC_String[m] != ','){
      RMC_Date += (char)RMC_String[m];
      m++;
    }
  }
    RMC_Date_output += "20";
    RMC_Date_output += RMC_Date[4];
    RMC_Date_output += RMC_Date[5];
    RMC_Date_output += '/';
    RMC_Date_output += RMC_Date[2];
    RMC_Date_output += RMC_Date[3];
    RMC_Date_output += '/';
    RMC_Date_output += RMC_Date[0];
    RMC_Date_output += RMC_Date[1];
    return (RMC_Date_output);
}

char RMC_N_S(String RMC_String){
  char RMC_NS;
  int m = 0;
  for(int n=0;n<5;n++){
    m++;
    while(RMC_String[m] != ','){
      RMC_NS = (char)RMC_String[m];
      m++;
    }
  }
  return (RMC_NS);
}

char RMC_E_W(String RMC_String){
  char RMC_EW;
  int m = 0;
  for(int n=0;n<7;n++){
    m++;
    while(RMC_String[m] != ','){
      RMC_EW = (char)RMC_String[m];
      m++;
    }
  }
  return (RMC_EW);
}

String RMC_Lat(String RMC_String){
  String RMC_lati = "";
  int m = 0;
  for(int n=0;n<4;n++){
    RMC_lati = "";
    m++;
    while(RMC_String[m] != ','){
      RMC_lati += (char)RMC_String[m];
      m++;
    }
  }
  return (RMC_lati);
}

String RMC_Lon(String RMC_String){
  String RMC_long = "";
  int m = 0;
  for(int n=0;n<6;n++){
    RMC_long = "";
    m++;
    while(RMC_String[m] != ','){
      RMC_long += (char)RMC_String[m];
      m++;
    }
  }
  return (RMC_long);
}

int RMC_Speed(String RMC_String){
  int speed_kmh = 0;
  String RMC_s = "";
  int m = 0;
  int n = 0;
  for(int n=0;n<8;n++){
    RMC_s = "";
    m++;
    while(RMC_String[m] != ','){
      RMC_s += (char)RMC_String[m];
      m++;
    }
  }
  while(RMC_s != "" && RMC_s[n] != '.'){
    n++;
  }
  if(n == 0){
    speed_kmh = 0;
    }
  else if(n == 1){
    speed_kmh = ((RMC_s[0]-48)*1)+((RMC_s[2]-48)*0.1)+((RMC_s[3]-48)*0.01);
  }else if(n == 2){
    speed_kmh = ((RMC_s[0]-48)*10)+((RMC_s[1]-48)*1)+((RMC_s[3]-48)*0.1)+((RMC_s[4]-48)*0.01);
  }
  else if(n == 3){
    speed_kmh = ((RMC_s[0]-48)*100)+((RMC_s[1]-48)*10)+((RMC_s[2]-48)*1)+((RMC_s[4]-48)*0.1)+((RMC_s[5]-48)*0.01);
  }
  return (speed_kmh*1.852);
}

String RMC_Angle(String RMC_String){
  String RMC_a = "";
  int m = 0;
  for(int n=0;n<9;n++){
    RMC_a = "";
    m++;
    while(RMC_String[m] != ','){
      RMC_a += (char)RMC_String[m];
      m++;
    }
  }
  return (RMC_a);
}

int lon_1(String lonString){
  int m = 0;
  int num;
  int num1;
  int num2;
  int num3;
  while(lonString != "" && lonString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = lonString[0] - 48;
    num2 = lonString[1] - 48;
    num = num1 * 10 + num2 * 1;
  }else if(m == 5){
    num1 = lonString[0] - 48;
    num2 = lonString[1] - 48;
    num3 = lonString[2] - 48;
    num = num1 * 100 + num2 * 10 + num3 * 1;
  }
  return (num);
}

int lon_2(String lonString){
  int m = 0;
  int num;
  int num1;
  int num2;
  while(lonString != "" && lonString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = lonString[2] - 48;
    num2 = lonString[3] - 48;
    num = num1 * 10 + num2 * 1;
  }else if(m == 5){
    num1 = lonString[3] - 48;
    num2 = lonString[4] - 48;
    num = num1 * 10 + num2 * 1;
  }
  return (num);
}

int lon_3(String lonString){
  int m = 0;
  float num;
  int num1;
  int num2;
  int num3;
  int num4;
  while(lonString != "" && lonString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = lonString[6] - 48;
    num2 = lonString[7] - 48;
    num3 = lonString[8] - 48;
    num4 = lonString[9] - 48;
    num = num1 * 0.1 + num2 * 0.01 + num3 * 0.001 + num4 * 0.0001;
  }else if(m == 5){
    num1 = lonString[7] - 48;
    num2 = lonString[8] - 48;
    num3 = lonString[9] - 48;
    num4 = lonString[10] - 48;
    num = num1 * 0.1 + num2 * 0.01 + num3 * 0.001 + num4 * 0.0001;
  }
  num = num * 60;
  return ((int)num);
}

int lat_1(String latString){
  int m = 0;
  int num;
  int num1;
  int num2;
  int num3;
  while(latString != "" && latString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = latString[0] - 48;
    num2 = latString[1] - 48;
    num = num1 * 10 + num2 * 1;
  }else if(m == 5){
    num1 = latString[0] - 48;
    num2 = latString[1] - 48;
    num3 = latString[2] - 48;
    num = num1 * 100 + num2 * 10 + num3 * 1;
  }
  return (num);
}

int lat_2(String latString){
  int m = 0;
  int num;
  int num1;
  int num2;
  while(latString != "" && latString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = latString[2] - 48;
    num2 = latString[3] - 48;
    num = num1 * 10 + num2 * 1;
  }else if(m == 5){
    num1 = latString[3] - 48;
    num2 = latString[4] - 48;
    num = num1 * 10 + num2 * 1;
  }
  return (num);
}

int lat_3(String latString){
  int m = 0;
  float num;
  int num1;
  int num2;
  int num3;
  int num4;
  while(latString != "" && latString[m] != '.'){
    m++;
  }
  if(m == 0){
    num = 0;
  }else if(m == 4){
    num1 = latString[6] - 48;
    num2 = latString[7] - 48;
    num3 = latString[8] - 48;
    num4 = latString[9] - 48;
    num = num1 * 0.1 + num2 * 0.01 + num3 * 0.001 + num4 * 0.0001;
  }else if(m == 5){
    num1 = latString[7] - 48;
    num2 = latString[8] - 48;
    num3 = latString[9] - 48;
    num4 = latString[10] - 48;
    num = num1 * 0.1 + num2 * 0.01 + num3 * 0.001 + num4 * 0.0001;
  }
    num = num * 60;
  return ((int)num);
}

int time_zone(int num1, int num2){
  float lon_int;
  lon_int = num1 + num2 * 0.01;
  if(lon_int < 7.5){
    return 0;
  }else if(lon_int >= 7.5 && lon_int <22.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 1;
  }else if(lon_int >= 22.5 && lon_int < 37.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 2;
  }else if(lon_int >= 37.5 && lon_int < 52.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 3;
  }else if(lon_int >= 52.5 && lon_int < 67.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 4;
  }
  else if(lon_int >= 67.5 && lon_int < 82.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 5;
  }
  else if(lon_int >= 82.5 && lon_int < 97.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 6;
  }
  else if(lon_int >= 97.5 && lon_int < 112.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 7;
  }
  else if(lon_int >= 112.5 && lon_int < 127.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 8;
  }
  else if(lon_int >= 127.5 && lon_int < 142.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 9;
  }
  else if(lon_int >= 142.5 && lon_int < 157.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 10;
  }
  else if(lon_int >= 157.5 && lon_int <= 172.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'E'){
    return 11;
  }else if(lon_int > 172.5){
    return 12;
  }
  else if(lon_int >= 7.5 && lon_int <22.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -1;
  }else if(lon_int >= 22.5 && lon_int < 37.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -2;
  }else if(lon_int >= 37.5 && lon_int < 52.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -3;
  }else if(lon_int >= 52.5 && lon_int < 67.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -4;
  }
  else if(lon_int >= 67.5 && lon_int < 82.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -5;
  }
  else if(lon_int >= 82.5 && lon_int < 97.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -6;
  }
  else if(lon_int >= 97.5 && lon_int < 112.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -7;
  }
  else if(lon_int >= 112.5 && lon_int < 127.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -8;
  }
  else if(lon_int >= 127.5 && lon_int < 142.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -9;
  }
  else if(lon_int >= 142.5 && lon_int < 157.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -10;
  }
  else if(lon_int >= 157.5 && lon_int <= 172.5 && RMC_E_W(Save_Data.GPS_Buffer) == 'W'){
    return -11;
  }
  else return 0;
}

void draw_time(){
  u8g2.setFont(u8g_font_unifont);
  u8g2.drawStr(0,11,"UTC Time:"); 
  u8g2.drawStr(0,63,"Time Zone:");
  u8g2.drawStr(82,62,"UTC+"); 
  u8g2.setFont(u8g_font_unifont); 
  u8g2.setCursor(23,29);
  u8g2.print(RMC_date(Save_Data.GPS_Buffer));
  u8g2.setCursor(29,44);
  u8g2.print(RMC_time(Save_Data.GPS_Buffer));
  u8g2.setCursor(114,62);
  u8g2.print(time_zone(lon_1(RMC_Lon(Save_Data.GPS_Buffer)),lon_2(RMC_Lon(Save_Data.GPS_Buffer))));
}

void draw_Detail(){
  char Output_second = '"' ;
  u8g2.setFont(u8g_font_unifont);
  u8g2.drawStr(0,12,"Lon:");
  u8g2.drawStr(0,27,"Lat:");
  u8g2.drawStr(0,42,"Speed:"); 
  u8g2.drawStr(0,57,"Angle:");
  u8g2.drawStr(95,42,"km/h");
  u8g2.drawStr(80,12,"'");
  u8g2.drawStr(80,27,"'");
  u8g2.setCursor(109,12);
  u8g2.print(Output_second);
  u8g2.setCursor(109,27);
  u8g2.print(Output_second);
  u8g2.setCursor(56,12);
  u8g2.print(char(176));
  u8g2.setCursor(56,27);
  u8g2.print(char(176));
  u8g2.setCursor(120,12);
  u8g2.print(RMC_E_W(Save_Data.GPS_Buffer));
  u8g2.print(char(176));
  u8g2.setCursor(120,27);
  u8g2.print(RMC_N_S(Save_Data.GPS_Buffer));
  u8g2.setFont(u8g_font_unifont); 
  u8g2.setCursor(49,42);
  u8g2.print(RMC_Speed(Save_Data.GPS_Buffer));
  u8g2.setCursor(49,57);
  u8g2.print(RMC_Angle(Save_Data.GPS_Buffer));
  u8g2.setCursor(34,12);
  u8g2.print(lon_1(RMC_Lon(Save_Data.GPS_Buffer)));
  u8g2.setCursor(66,12);
  u8g2.print(lon_2(RMC_Lon(Save_Data.GPS_Buffer)));
  u8g2.setCursor(88,12);
  u8g2.print(lon_3(RMC_Lon(Save_Data.GPS_Buffer)));
  u8g2.setCursor(34,27);
  u8g2.print(lat_1(RMC_Lat(Save_Data.GPS_Buffer)));
  u8g2.setCursor(66,27);
  u8g2.print(lat_2(RMC_Lat(Save_Data.GPS_Buffer)));
  u8g2.setCursor(88,27);
  u8g2.print(lat_3(RMC_Lat(Save_Data.GPS_Buffer)));
}

void background_led(){
  if(((button_m == 0 && button_n == 0)||(button_m == -1 && button_n == -1)) && (digitalRead(8) == LOW)){
    button_m = 0;
    button_n = 0;analogWrite(9,0);
    //开背光
  }else if( ((button_m == 0 && button_n == 0)||(button_m == 1 && button_n == 0)) && digitalRead(8) == HIGH){
    //关背光
    analogWrite(9,169);
    button_m = 1;
  }else if( (button_m == 1 && button_n == 0) || (button_m == 1 && button_n ==1) && digitalRead(8) == LOW ){
    //关背光
    analogWrite(9,169);
    button_n = 1;
  }else if(((button_n == 1 && button_m == 1) || (button_n == -1 && button_m == -1)) && digitalRead(8) == HIGH){
    button_m = -1;
    button_n = -1;
    //开背光
    analogWrite(9,0);
  }
}

void draw_Error(){
  u8g2.setFont(u8g_font_unifont);
  u8g2.drawStr(0,27,"Error");     //显示"Error"
}

void clrGpsRxBuffer(void)
{
  memset(gpsRxBuffer, 0, gpsRxBufferLength);
  ii = 0;
}

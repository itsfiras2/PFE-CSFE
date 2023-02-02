const char* ssid = "USMO";           //your network SSID
const char* password = "Firas1221";  //your network password

///////////////////////////////////////////////////////////////////////////
const char* apssid = "ESP32-CAM";
const char* appassword = "12345678";  //AP端密碼至少要八個字元以上
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"        //video
#include "soc/soc.h"           //ki tebda cnx khayba ca sera redémarrer
#include "soc/rtc_cntl_reg.h"  //nafs la7keya

String Feedback = "";  // hedhi traja3 msg ll client
String Command = "", cmd = "", P1 = "", P2 = "", P3 = "", P4 = "", P5 = "", P6 = "", P7 = "", P8 = "", P9 = "";

byte ReceiveState = 0, cmdState = 1, strState = 1, questionstate = 0, equalstate = 0, semicolonstate = 0;

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22


WiFiServer server(80);

void ExecuteCommand() 
{
  //Serial.println("");
  //Serial.println("Command: "+Command);
  if (cmd != "getstill") {
    Serial.println("cmd= " + cmd + " ,P1= " + P1 + " ,P2= " + P2 + " ,P3= " + P3 + " ,P4= " + P4 + " ,P5= " + P5 + " ,P6= " + P6 + " ,P7= " + P7 + " ,P8= " + P8 + " ,P9= " + P9);
    Serial.println("");
  }

  if (cmd == "serievalide") {
    digitalWrite(2, LOW);  //Turn on

  } else if (cmd == "ip") {
    Feedback = "AP IP: " + WiFi.softAPIP().toString();
    Feedback += ", ";
    Feedback += "STA IP: " + WiFi.localIP().toString();
  } else if (cmd == "mac") {
    Feedback = "STA MAC: " + WiFi.macAddress();
  } else if (cmd == "resetwifi") {  // redemarrage wifi
    WiFi.begin(P1.c_str(), P2.c_str());
    Serial.print("Connecting to ");
    Serial.println(P1);
    long int StartTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if ((StartTime + 5000) < millis()) break;
    }
    Serial.println("");
    Serial.println("STAIP: " + WiFi.localIP().toString());
    Feedback = "STAIP: " + WiFi.localIP().toString();
  } else if (cmd == "restart") {
    ESP.restart();
  } else if (cmd == "digitalwrite") {
    ledcDetachPin(P1.toInt());
    pinMode(P1.toInt(), OUTPUT);
    digitalWrite(P1.toInt(), P2.toInt());
    digitalWrite(2, HIGH);

  }

  else if (cmd == "analogwrite") {
    Serial.println(P2);
    if (P1 = "4") {
      ledcAttachPin(4, 4);
      ledcSetup(4, 5000, 8);
      ledcWrite(4, P2.toInt());
      digitalWrite(2, HIGH);
    } else {
      ledcAttachPin(P1.toInt(), 5);
      ledcSetup(5, 5000, 8);
      ledcWrite(5, P2.toInt());
      digitalWrite(2, HIGH);
    }
  } else if (cmd == "flash") {
    ledcAttachPin(4, 4);
    ledcSetup(4, 5000, 8);

    int val = P1.toInt();
    ledcWrite(4, val);
  } else if (cmd == "framesize") {
    sensor_t* s = esp_camera_sensor_get();
    if (P1 == "QQVGA")
      s->set_framesize(s, FRAMESIZE_QQVGA);
    else if (P1 == "HQVGA")
      s->set_framesize(s, FRAMESIZE_HQVGA);
    else if (P1 == "QVGA")
      s->set_framesize(s, FRAMESIZE_QVGA);
    else if (P1 == "CIF")
      s->set_framesize(s, FRAMESIZE_CIF);
    else if (P1 == "VGA")
      s->set_framesize(s, FRAMESIZE_VGA);
    else if (P1 == "SVGA")
      s->set_framesize(s, FRAMESIZE_SVGA);
    else if (P1 == "XGA")
      s->set_framesize(s, FRAMESIZE_XGA);
    else if (P1 == "SXGA")
      s->set_framesize(s, FRAMESIZE_SXGA);
    else if (P1 == "UXGA")
      s->set_framesize(s, FRAMESIZE_UXGA);
    else
      s->set_framesize(s, FRAMESIZE_QVGA);
  } else if (cmd == "quality") {
    sensor_t* s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_quality(s, val);
  } else if (cmd == "contrast") {
    sensor_t* s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_contrast(s, val);
  } else if (cmd == "brightness") {
    sensor_t* s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_brightness(s, val);
  } else {
    Feedback = "Command is not defined.";
  }
  if (Feedback == "") Feedback = Command;
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //en cas où 9ass edhaw
  Serial.begin(115200);
  Serial.setDebugOutput(true);  //
  Serial.println();

  //oumour sortie video
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //drop down frame size for higher initial frame rate
  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);  //UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  //hedhi fonction ta3mel taswira mel camera mta3 l esp32
  ledcAttachPin(4, 4);
  ledcSetup(4, 5000, 8);

  WiFi.mode(WIFI_AP_STA);

  //ip static :
  //WiFi.config(IPAddress(192, 168, 201, 100), IPAddress(192, 168, 201, 2), IPAddress(255, 255, 255, 0));

  WiFi.begin(ssid, password);  // 3ammel 3la rabi

  delay(1000);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  long int StartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((StartTime + 10000) < millis()) break;  //10 secs de repos
  }

  if (WiFi.status() == WL_CONNECTED) {  //sahhel rabi w connectina
    WiFi.softAP((WiFi.localIP().toString() + "_" + (String)apssid).c_str(), appassword);
    Serial.println("");
    Serial.println("STAIP address: ");
    Serial.println(WiFi.localIP());

    for (int i = 0; i < 5; i++) {  //flash ( chohera dhaw led)
      ledcWrite(4, 10);
      delay(200);
      ledcWrite(4, 0);
      delay(200);
    }
  } else {
    WiFi.softAP((WiFi.softAPIP().toString() + "_" + (String)apssid).c_str(), appassword);

    for (int i = 0; i < 2; i++) {  //led slow motion
      ledcWrite(4, 10);
      delay(1000);
      ledcWrite(4, 0);
      delay(1000);
    }
  }

  //WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  Serial.println("");
  Serial.println("APIP address: ");
  Serial.println(WiFi.softAPIP());

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  server.begin();
}

//Page html
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
  <head>
  <title>PFF FIRAS</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <script src="https:\/\/ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js"></script>
  <script src="https:\/\/unpkg.com/tesseract.js/dist/tesseract.min.js"></script>
  <style>
    button { 
        font-family:cursive;
        text-decoration: none;
        font-size: 22px;
        background: rgb(242,112,156);
background: linear-gradient(90deg, rgba(242,112,156,1) 0%, rgba(255,148,114,1) 100%);
        margin-top: 10px;
        color: antiquewhite;
        border-radius: 5px;
        border-style:dotted;
        
    }
    #restart {
        text-decoration: none;
        text-shadow: 3px;
        font-size: 15px;
        font-family: cursive;
        background-color: red;
        color:azure;
        border-radius:5px;
        border-style: groove;
        border: none;
    }
    #getStill {
        
        text-decoration: none;
        text-shadow: 3px;
        font-size: 15px;
        font-family: cursive;
        background-color:green;
        color:azure;
        border-radius:5px;
        border-style: groove;
        border: none;
    }
    #result {
        text-decoration:dashed;
        font-size:20px;
        margin-top: 20px;
        font-family: Cambria, Cochin, Georgia, Times, 'Times New Roman', serif;
    }
  </style>
  </head>
  <body>
  <img id="ShowImage" src="">
  <br>
    <table>
  <tr>
    <td><input type="button" id="restart" value="Restart"></td>
    <td > 
    </table>
    <input type="button" id="getStill" value="Afficher La Matricule"></td> 
  </tr>
  <br>
  <button id="btnDetect" onclick="TextRecognition();" disabled >Verifier La Matricule</button>
  <br>
  <div id="result"></div>
  <script>
    var getStill = document.getElementById('getStill');
    var ShowImage = document.getElementById('ShowImage');
    var result = document.getElementById('result');
    var flash = document.getElementById('flash');
    var btnDetect = document.getElementById('btnDetect');
    var myTimer;
    var restartCount=0;    
    getStill.onclick = function (event) {  
      clearInterval(myTimer);  
      myTimer = setInterval(function(){error_handle();},5000);
      ShowImage.src=location.origin+'/?getstill='+Math.random();      
    }
    function error_handle() {
      btnDetect.disabled =true;
      restartCount++;
      clearInterval(myTimer);
      if (restartCount<=2) {
        result.innerHTML = "Get still error. <br>Restart ESP32-CAM "+restartCount+" times.";
        myTimer = setInterval(function(){getStill.click();},10000);
      }
      else
        result.innerHTML = "Get still error. <br>Please close the page and check ESP32-CAM.";
    }  
    ShowImage.onload = function (event) {
      btnDetect.disabled =false;
      clearInterval(myTimer);
      restartCount=0; 
      getStill.click();
    }         
    restart.onclick = function (event) {
      fetch(location.origin+'?restart=stop');
    }    
    framesize.onclick = function (event) {
      fetch(document.location.origin+'?framesize='+this.value+';stop');
    }  
    flash.onchange = function (event) {
      fetch(location.origin+'?flash='+this.value+';stop');
    } 
    quality.onclick = function (event) {
      fetch(document.location.origin+'?quality='+this.value+';stop');
    } 
    brightness.onclick = function (event) {
      fetch(document.location.origin+'?brightness='+this.value+';stop');
    } 
    contrast.onclick = function (event) {
      fetch(document.location.origin+'?contrast='+this.value+';stop');
    }                             
    
    function getFeedback(target) {
      var data = $.ajax({
      type: "get",
      dataType: "text",
      url: target,
      success: function(response)
        {
          result.innerHTML = response;
        },
        error: function(exception)
        {
          result.innerHTML = 'fail';
        }
      });
    }   
    function TextRecognition() {
      result.innerHTML = "Reconnaisance en cours ...";
      Tesseract.recognize(
        ShowImage,
        lang.value,
        { logger: m => console.log(m) }
      ).then(({ data: { text } }) => {
  
        result.innerHTML = text.replace(/\n/g, "<br>");
        text = result.innerHTML.replace(/<br>/g, "");
        if  (text=="12345678"||text=="A"||text=="FIRAS"||text=="2023"||text=="5711"||text=="198") {
          $.ajax({url: document.location.origin+'?analogwrite=4;10', async: false});
            $.ajax({url: document.location.origin+'?serievalide=4;10', async: false});          
        }
        else if (text=="b"||text=="B") {
          $.ajax({url: document.location.origin+'?analogwrite=4;0', async: false});
          //$.ajax({url: document.location.origin+'?digitalwrite=2;0', async: false});
        }
      })  
    }     

  </script>
  </body>
  </html> 
  )rawliteral";
void loop() {
  Feedback = "";
  Command = "";
  cmd = "";
  P1 = "";
  P2 = "";
  P3 = "";
  P4 = "";
  P5 = "";
  P6 = "";
  P7 = "";
  P8 = "";
  P9 = "";
  ReceiveState = 0, cmdState = 1, strState = 1, questionstate = 0, equalstate = 0, semicolonstate = 0;

  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        getCommand(c);  //el text eli jebou m taswira

        if (c == '\n') {
          if (currentLine.length() == 0) {

            if (cmd == "getstill") {
              //to jpg extension
              camera_fb_t* fb = NULL;
              fb = esp_camera_fb_get();
              if (!fb) {
                Serial.println("Camera capture failed");
                delay(1000);
                ESP.restart();
              }

              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: image/jpeg");
              client.println("Content-Disposition: form-data; name=\"imageFile\"; filename=\"picture.jpg\"");
              client.println("Content-Length: " + String(fb->len));
              client.println("Connection: close");
              client.println();

              uint8_t* fbBuf = fb->buf;
              size_t fbLen = fb->len;
              for (size_t n = 0; n < fbLen; n = n + 1024) {
                if (n + 1024 < fbLen) {
                  client.write(fbBuf, 1024);
                  fbBuf += 1024;
                } else if (fbLen % 1024 > 0) {
                  size_t remainder = fbLen % 1024;
                  client.write(fbBuf, remainder);
                }
              }

              esp_camera_fb_return(fb);

              pinMode(4, OUTPUT);
              digitalWrite(4, LOW);
            } else {
              // bech yrender el page html
              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: text/html; charset=utf-8");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Connection: close");
              client.println();

              String Data = "";
              if (cmd != "")
                Data = Feedback;
              else {
                Data = String((const char*)INDEX_HTML);
              }
              int Index;
              for (Index = 0; Index < Data.length(); Index = Index + 1000) {
                client.print(Data.substring(Index, Index + 1000));
              }

              client.println();
            }

            Feedback = "";
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if ((currentLine.indexOf("/?") != -1) && (currentLine.indexOf(" HTTP") != -1)) {
          if (Command.indexOf("stop") != -1) {  //hedhi ki twarih kelmet stop ye9f  -> http://192.168.xxx.xxx/?cmd=aaa;bbb;ccc;stop
            client.println();
            client.println();
            client.stop();
          }
          currentLine = "";
          Feedback = "";
          ExecuteCommand();
        }
      }
    }
    delay(1);
    client.stop();
  }
}

void getCommand(char c) {
  if (c == '?') ReceiveState = 1;
  if ((c == ' ') || (c == '\r') || (c == '\n')) ReceiveState = 0;

  if (ReceiveState == 1) {
    Command = Command + String(c);

    if (c == '=') cmdState = 0;
    if (c == ';') strState++;

    if ((cmdState == 1) && ((c != '?') || (questionstate == 1))) cmd = cmd + String(c);
    if ((cmdState == 0) && (strState == 1) && ((c != '=') || (equalstate == 1))) P1 = P1 + String(c);
    if ((cmdState == 0) && (strState == 2) && (c != ';')) P2 = P2 + String(c);
    if ((cmdState == 0) && (strState == 3) && (c != ';')) P3 = P3 + String(c);
    if ((cmdState == 0) && (strState == 4) && (c != ';')) P4 = P4 + String(c);
    if ((cmdState == 0) && (strState == 5) && (c != ';')) P5 = P5 + String(c);
    if ((cmdState == 0) && (strState == 6) && (c != ';')) P6 = P6 + String(c);
    if ((cmdState == 0) && (strState == 7) && (c != ';')) P7 = P7 + String(c);
    if ((cmdState == 0) && (strState == 8) && (c != ';')) P8 = P8 + String(c);
    if ((cmdState == 0) && (strState >= 9) && ((c != ';') || (semicolonstate == 1))) P9 = P9 + String(c);

    if (c == '?') questionstate = 1;
    if (c == '=') equalstate = 1;
    if ((strState >= 9) && (c == ';')) semicolonstate = 1;
  }
}

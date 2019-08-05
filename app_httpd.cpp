// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//URL details:
//  /control?var=framesize&val=0

#include "esp_http_server.h"
#include "esp_camera.h"
#include "Arduino.h"

#define ledPin 4 //Pin for built-in LED flash
#define RELAY_PIN 13 //Relay Pin for Printer Mains Relay

//Start Config
uint8_t debugmsg = 0;  //Debug Serial Messages
int PrintSerial_Speed = 250000; //Speed for Serial connection to Printer - Ender 3 default is 115200
#define SERIAL1_RXPIN 14 //Serial Pin for PrinterSerial
#define SERIAL1_TXPIN 15 //Serial Pin for PrinterSerial
String abortString = "M117 Print Aborted\nM25\nM410\nG91\nG0 Z10\n\nG0 E-5\nM400\nG90\nM104 S0\nM140 S0\nM106 S0\nG0 X0 Y220\nM18\n"; //GCode for doing an aborting a print.
//End Config

 
#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();



unsigned char h2int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return ((unsigned char)c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return ((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F')
    {
        return ((unsigned char)c - 'A' + 10);
    }
    return (0);
}

String urldecode(String str)
{

    String encodedString = "";
    char c;
    char code0;
    char code1;
    for (int i = 0; i < str.length(); i++)
    {
        c = str.charAt(i);
        if (c == '+')
        {
            encodedString += ' ';
        }
        else if (c == '%')
        {
            i++;
            code0 = str.charAt(i);
            i++;
            code1 = str.charAt(i);
            c = (h2int(code0) << 4) | h2int(code1);
            encodedString += c;
        }
        else
        {

            encodedString += c;
        }

        yield();
    }

    return encodedString;
}

static const char PROGMEM INDEX2_HTML[] = R"rawliteral(
<!doctype html>
<html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>ESP32-CAM Print Monitor</title>
        <style>
body{font-family:Arial,Helvetica,sans-serif;background:#181818;color:#efefef;font-size:16px}h2{font-size:18px}#menu,section.main{flex-direction:column}section.main{display:flex}#menu{display:none;flex-wrap:nowrap;min-width:340px;background:#666;padding:8px;border-radius:4px;margin-top:-10px;margin-right:10px}#content{display:flex;flex-wrap:wrap;align-items:stretch}figure{padding:0;margin:0;-webkit-margin-before:0;margin-block-start:0;-webkit-margin-after:0;margin-block-end:0;-webkit-margin-start:0;margin-inline-start:0;-webkit-margin-end:0;margin-inline-end:0}figure img{display:block;width:100%;height:auto;border-radius:4px;margin-top:8px}@media (min-width:800px) and (orientation:landscape){#content{display:flex;flex-wrap:nowrap;align-items:stretch}figure img{display:block;max-width:100%;max-height:calc(100vh - 40px);width:auto;height:auto}figure{padding:0;margin:0;-webkit-margin-before:0;margin-block-start:0;-webkit-margin-after:0;margin-block-end:0;-webkit-margin-start:0;margin-inline-start:0;-webkit-margin-end:0;margin-inline-end:0}}section#buttons{display:flex;flex-wrap:nowrap;justify-content:space-between}#nav-toggle{cursor:pointer;display:block}#nav-toggle-cb{outline:0;opacity:0;width:0;height:0}#nav-toggle-cb:checked+#menu{display:flex}.input-group{display:flex;flex-wrap:nowrap;line-height:22px;margin:5px 0}.input-group>label{display:inline-block;padding-right:10px;min-width:47%}.input-group input,.input-group select{flex-grow:1}.range-max,.range-min{display:inline-block;padding:0 5px}button{display:block;margin:5px;padding:0 12px;border:0;line-height:28px;cursor:pointer;color:#fff;background:#ff3034;border-radius:5px;font-size:16px;outline:0}button:hover{background:#ff494d}button:active{background:#f21c21}button.disabled{cursor:default;background:#a0a0a0}input[type=range]{-webkit-appearance:none;width:100%;height:22px;background:#363636;cursor:pointer;margin:0}input[type=range]:focus{outline:0}input[type=range]::-webkit-slider-runnable-track{width:100%;height:2px;cursor:pointer;background:#efefef;border-radius:0;border:0 solid #efefef}input[type=range]::-webkit-slider-thumb{border:1px solid transparent;height:22px;width:22px;border-radius:50px;background:#ff3034;cursor:pointer;-webkit-appearance:none;margin-top:-11.5px}input[type=range]:focus::-webkit-slider-runnable-track{background:#efefef}input[type=range]::-moz-range-track{width:100%;height:2px;cursor:pointer;background:#efefef;border-radius:0;border:0 solid #efefef}input[type=range]::-moz-range-thumb{border:1px solid transparent;height:22px;width:22px;border-radius:50px;background:#ff3034;cursor:pointer}input[type=range]::-ms-track{width:100%;height:2px;cursor:pointer;background:0 0;border-color:transparent;color:transparent}input[type=range]::-ms-fill-lower{background:#efefef;border:0 solid #efefef;border-radius:0}input[type=range]::-ms-fill-upper{background:#efefef;border:0 solid #efefef;border-radius:0}input[type=range]::-ms-thumb{border:1px solid transparent;width:22px;border-radius:50px;background:#ff3034;cursor:pointer;height:2px}input[type=range]:focus::-ms-fill-lower{background:#efefef}input[type=range]:focus::-ms-fill-upper{background:#363636}.switch{line-height:22px;font-size:16px}.switch input{outline:0;opacity:0;width:0;height:0}.slider,.slider:before{width:50px;height:22px;border-radius:22px;display:inline-block}.slider:before{border-radius:50%;top:3px;position:absolute;content:"";height:15px;width:15px;left:4px;bottom:4px;background-color:#fff;-webkit-transition:.4s;transition:.4s}input:checked+.slider{background-color:#2196f3}input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}select{border:1px solid #363636;font-size:14px;height:22px;outline:0;border-radius:5px}.image-container{position:relative;min-width:160px}.abortbtn,.powerbtn{position:absolute;right:5px;top:5px;background-color:rgba(255,48,52,.5);color:#fff;text-align:center}.powerbtn{top:40px}.bedtemp,.elapsedtime,.exttemp,.proglabel,.cputemp{font-family:Arial,Helvetica,sans-serif;font-size:10px;position:absolute;left:5px;top:5px;background-color:rgba(140,114,114,.5);color:#fff;text-align:center}.bedtemp,.elapsedtime,.exttemp{top:23px}.bedtemp,.exttemp{top:40px}.bedtemp{top:60px}.cputemp{top:80px}.hidden{display:none}.lightsw{position:absolute;right:5px;top:85px;text-align:center;font-family:Arial,Helvetica,sans-serif}.streamsw{position:absolute;right:5px;top:130px;text-align:center;font-family:Arial,Helvetica,sans-serif}.switch{position:relative;display:inline-block;width:60px;height:34px}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s}input:focus+.slider{box-shadow:0 0 1px #2196f3}.slider.round{border-radius:34px}.slider.round:before{border-radius:50%}
        </style>
                <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
        <script>
function load() {
//document.getElementById('stream').src = window.location.protocol + "//" + window.location.hostname + ":9601/stream";
check();
var refreshrate = 60000;
var inst = setInterval(change, refreshrate);
var query = setInterval(check, refreshrate);

function change() {
        $.ajax({
          url: "status",
          method: "GET",
          success: function(data) {
            var temperature = data.temperature;
            var elem = document.getElementById("cpu-temp");
            elem.innerHTML = 'CPU: ' + Number.parseFloat(temperature).toFixed(2) + ' &#8451';
          },
          error: function(data) {
            console.log("Data Error");
          }
        });
console.log("change executed");
}



function check() {
        $.ajax({
          url: "control?var=query&val=1",
          method: "GET",
          success: function(data2) {
            var progress = data2.progress;
            var exttemp = data2.exttemp;
            var bedtemp = data2.bedtemp;
            var elapsedt = data2.elapsedt;
            var proglabel = document.getElementById("prog-label");
            proglabel.innerHTML = 'Progress: ' + progress + ' %';
            var exttemplabel = document.getElementById("ext-temp");
            exttemplabel.innerHTML = 'Extruder: ' + exttemp + ' &#8451';
            var bedtemplabel = document.getElementById("bed-temp");
            bedtemplabel.innerHTML = 'Bed: ' + bedtemp + ' &#8451';
            var elapsedlabel = document.getElementById("elapsed-time");
            elapsedlabel.innerHTML = 'Elapsed Time: ' + elapsedt; 
          },
          error: function(data2) {
            console.log("Data Error");
          }
        });
console.log("check executed refresh=" + refreshrate);
}

$('#lighttoggle').change(function(){
		if (document.getElementById('lighttoggle').checked) {
        $.ajax({
          type:'GET',
          url:'control?var=light&val=1',
          success:function(data)
          {
          }
        });
		} else {
			        $.ajax({
          type:'GET',
          url:'control?var=light&val=0',
          success:function(data)
          {
          }
        });
		}
      });
	  
$('#refreshrate').change(function(){


    var myCookie = getCookie("refreshrate");

    if (myCookie == null) {
    document.cookie = "refreshrate=60000";
        // do cookie doesn't exist stuff;
    }
    else {
    refreshrate = getCookie(refreshrate);
        // do cookie exists stuff
    }




refreshrate = document.getElementById('refreshrate').value;
document.cookie = "refreshrate=" + refreshrate;
clearInterval(inst);
inst = setInterval(change, refreshrate);
clearInterval(query);
query = setInterval(check, refreshrate);

      });	  
	  
	  $('#streamtoggle').change(function(){
		if (document.getElementById('streamtoggle').checked) {
       document.getElementById('stream').src = window.location.protocol + "//" + window.location.hostname + ":9601/stream";
		} else {
window.stop
document.getElementById('stream').src = "https://www.mapme.ga/ESP32-CAM-PM.jpg";
		}
      });
      
      function getCookie(name) {
    var dc = document.cookie;
    var prefix = name + "=";
    var begin = dc.indexOf("; " + prefix);
    if (begin == -1) {
        begin = dc.indexOf(prefix);
        if (begin != 0) return null;
    }
    else
    {
        begin += 2;
        var end = document.cookie.indexOf(";", begin);
        if (end == -1) {
        end = dc.length;
        }
    }
    // because unescape has been deprecated, replaced with decodeURI
    //return unescape(dc.substring(begin + prefix.length, end));
    return decodeURI(dc.substring(begin + prefix.length, end));
} 
	  
}

</script>
<script>
function abortClicked() {
     var x;
     if (confirm("Are you sure you want to abort the print?") == true) {
                 $.ajax({
          url: 'control?var=abort&val=1',
          method: "GET",
          success: function(data) {
 
          },
          error: function(data) {
            console.log("Data Error");
          }
        });
     } else {
         x = "You pressed Cancel!";
     }
     return x; 


}
     function powerClicked() {
     var x;
     if (confirm("Are you sure you want to power cycle the printer?") == true) {
                          $.ajax({
          url: 'control?var=shutdown&val=1',
          method: "GET",
          success: function(data) {
          
          },
          error: function(data) {
            console.log("Data Error");
          }
        });
     } else {
         x = "You pressed Cancel!";
     }
     return x; 
     }
	 
	      function sendClicked() {
     var x;
	 if (document.getElementById('gcode').value.length > 0){
	 x = document.getElementById('gcode').value + '\n';
	 x = encodeURI(x);
	 console.log(x);
	 document.getElementById('sendButton').disabled = true;
	 
	                           $.ajax({
          url: 'control?var=command&val=' + x,
          method: "GET",
          success: function(data3) {
              var cmdresponse = data3.cmdresponse;
              
          document.getElementById('gcode').value = "";
          document.getElementById('gcode').placeholder = cmdresponse;

		  document.getElementById('sendButton').disabled = false;
          },
          error: function(data) {
            console.log("Data Error");
			document.getElementById('sendButton').disabled = false;
          }
        });
} else {
  alert("No command entered..");
}

    
     }
</script>
    </head>
    <body onload="load()">
        <section class="main">

            <div id="content">
 

                    <figure>
                    <div id="stream-container" class="image-container" width="800" height="600">
                        <button class="abortbtn" id="abort-btn" onclick="abortClicked()">Abort</button>
                        <button class="powerbtn" id="power-btn" onclick="powerClicked()">Power</button>
                                    <div class="lightsw" id="light-sw">
                                    <label class="switch">
                                    <input type="checkbox" id ="lighttoggle">
                                    <span class="slider round"></span><br/>
                                    Light
                                    </label>
                                    </div>
                                    <div class="streamsw" id="stream-sw">
                                    <label class="switch">
                                    <input type="checkbox" id ="streamtoggle">
                                    <span class="slider round"></span><br/>
                                    Stream
                                    </label>
                                    </div>
                        <div class="proglabel" id="prog-label">Progress: Reading...           
                        </div>
                        <div class="elapsedtime" id="elapsed-time">Elapsed time: Reading...                        
                        </div>
                        <div class="exttemp" id="ext-temp">Extruder: Reading...                        
                        </div>
                        <div class="bedtemp" id="bed-temp">Bed: Reading...                        
                        </div>
                        <div class="cputemp" id="cpu-temp">CPU: Reading...                        
                        </div>
                        <img id="stream" src="https://www.mapme.ga/ESP32-CAM-PM.jpg">
                    </div>
                </figure>
            </div>
        </section>
        <input type="text" id="gcode" maxlength="25" size="40"><button id="sendButton" onclick="sendClicked()" placeholder="Enter GCode..">Send Code</button>
		                       <div id="logo">
                <label for="nav-toggle-cb" id="nav-toggle">&#9776;&nbsp;&nbsp;Toggle settings</label>
            </div>
			                                <div id="sidebar">
                    <input type="checkbox" id="nav-toggle-cb">
                    <nav id="menu">
                        <div class="input-group" id="framesize-group">
                            <label for="refreshrate">Refresh Rate</label>
                            <select id="refreshrate" class="default-action">
                                <option value="120000">120 seconds</option>
                                <option value="60000" selected="selected">60 seconds</option>
                                <option value="30000">30 seconds</option>
                                <option value="20000">20 seconds</option>
                                <option value="10000">10 seconds</option>
                            </select>
                        </div>

                        <section id="buttons">
                            <button id="get-still">Reboot ESP</button>
                        </section>
                    </nav>
                </div>
    </body>
</html>                     
)rawliteral";
///////////

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static int8_t light_enabled = 0;
static int8_t aborted = 0;
static int8_t shutdown = 0;

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len)
{
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index)
    {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
    {
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[256]; //used to be 64

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    while (true)
    {

        fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {

            if (fb->width > 400)
            {
                if (fb->format != PIXFORMAT_JPEG)
                {
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if (!jpeg_converted)
                    {
                        Serial.println("JPEG compression failed");
                        res = ESP_FAIL;
                    }
                }
                else
                {
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
            }
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 256, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            break;
        }
    }

    return res;
}

static esp_err_t cmd_handler(httpd_req_t *req)
{


    HardwareSerial PrintSerial(1);
    PrintSerial.begin(PrintSerial_Speed, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
    char *buf;
    size_t buf_len;
    char variable[32] = {
        0,
    };
    char value[32] = {
        0,
    };

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
                httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK)
            {
                Serial.println(value);
            }
            else
            {
                free(buf);
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        }
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    }
    else
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int val = atoi(value);
    sensor_t *s = esp_camera_sensor_get();
    int res = 0;

    if (!strcmp(variable, "framesize"))
    {

        if (s->pixformat == PIXFORMAT_JPEG)
            res = s->set_framesize(s, (framesize_t)val);
    }
    else if (!strcmp(variable, "quality"))
        res = s->set_quality(s, val);
    else if (!strcmp(variable, "contrast"))
        res = s->set_contrast(s, val);
    else if (!strcmp(variable, "brightness"))
        res = s->set_brightness(s, val);
    else if (!strcmp(variable, "saturation"))
        res = s->set_saturation(s, val);
    else if (!strcmp(variable, "gainceiling"))
        res = s->set_gainceiling(s, (gainceiling_t)val);
    else if (!strcmp(variable, "colorbar"))
        res = s->set_colorbar(s, val);
    else if (!strcmp(variable, "awb"))
        res = s->set_whitebal(s, val);
    else if (!strcmp(variable, "agc"))
        res = s->set_gain_ctrl(s, val);
    else if (!strcmp(variable, "aec"))
        res = s->set_exposure_ctrl(s, val);
    else if (!strcmp(variable, "hmirror"))
        res = s->set_hmirror(s, val);
    else if (!strcmp(variable, "vflip"))
        res = s->set_vflip(s, val);
    else if (!strcmp(variable, "awb_gain"))
        res = s->set_awb_gain(s, val);
    else if (!strcmp(variable, "agc_gain"))
        res = s->set_agc_gain(s, val);
    else if (!strcmp(variable, "aec_value"))
        res = s->set_aec_value(s, val);
    else if (!strcmp(variable, "aec2"))
        res = s->set_aec2(s, val);
    else if (!strcmp(variable, "dcw"))
        res = s->set_dcw(s, val);
    else if (!strcmp(variable, "bpc"))
        res = s->set_bpc(s, val);
    else if (!strcmp(variable, "wpc"))
        res = s->set_wpc(s, val);
    else if (!strcmp(variable, "raw_gma"))
        res = s->set_raw_gma(s, val);
    else if (!strcmp(variable, "lenc"))
        res = s->set_lenc(s, val);
    else if (!strcmp(variable, "special_effect"))
        res = s->set_special_effect(s, val);
    else if (!strcmp(variable, "wb_mode"))
        res = s->set_wb_mode(s, val);
    else if (!strcmp(variable, "ae_level"))
    {

        Serial.printf("var=");
        Serial.printf(variable);

        res = s->set_ae_level(s, val);
        Serial.printf(" valvar=");
        Serial.print(val);
    }
    else if (!strcmp(variable, "light"))
    {
        light_enabled = val;
        if (light_enabled)
        {
            digitalWrite(ledPin, HIGH);
        }
        else
        {
            digitalWrite(ledPin, LOW);
        }
    }
    else if (!strcmp(variable, "reboot"))
    {
        // light_enabled = val;
        if (val)
        {
            if (debugmsg)
            {
                Serial.println("Restarting ESP");
            }
            ESP.restart();
        }
        else
        {
            // digitalWrite(ledPin, LOW);
        }
    }
    else if (!strcmp(variable, "query"))
    {

        String a, part1, part2, exttemp, bedtemp, elapsedt, firstHalf, secondHalf;
        float progress, part1long, part2long;
        int ind1, bedindex, lastindex, extindex, lastind;

        if (debugmsg)
        {
            Serial.println("Start query");
        }
        PrintSerial.print("M27\n"); //SD printing byte XXXXXX/XXXXXXX
        if (debugmsg)
        {
            Serial.println("M27 Sent");
        }
        delay(1000);
        while (PrintSerial.available())
        {

            a = PrintSerial.readStringUntil('\n'); // read the incoming data as string

            if (a.startsWith("SD printing byte "))
            { //Printing progress response

                a.remove(0, 17);
                if (debugmsg)
                {
                    Serial.println("Printing progress Triggered");
                }
                ind1;
                ind1 = a.indexOf('/');
                part1 = a.substring(0, ind1);
                part2 = a.substring(ind1 + 1);
                part1long = part1.toFloat();
                part2long = part2.toFloat();
                progress = (part1long / part2long) * 100;
            }
            else if (a.startsWith("ok T"))
            { //Temperature response
                if (debugmsg)
                {
                    Serial.println("Temperature response Triggered");
                }
                a.remove(0, 5);
                bedindex = a.indexOf(":");
                secondHalf = a.substring(bedindex, a.length());

                secondHalf.remove(0, 1);
                lastindex = secondHalf.indexOf(" ");
                secondHalf.remove(lastindex);
                extindex = a.indexOf(" ");
                firstHalf = a;
                firstHalf.remove(extindex);
                // a.remove(tempstring);
                if (debugmsg)
                {
                    Serial.println("Ext temp:" + firstHalf);
                    Serial.println("Bed temp:" + secondHalf);
                }
                exttemp = firstHalf;
                bedtemp = secondHalf;
            }
            else if (a.startsWith("echo"))
            {
                if (debugmsg)
                {
                    Serial.println("Elapsed Time Response Triggered");
                }
                lastind = a.lastIndexOf(":");
                a.remove(0, lastind);
                a.remove(0, 2);
                elapsedt = a;
            }
            else
            {
                if (debugmsg)
                {
                    Serial.println("Unknown response: " + a);
                }
            }
        }
        PrintSerial.print("M31\n"); //echo: Print time:  XXh XXm XXs
        if (debugmsg)
        {
            Serial.println("M31 Sent");
        }
        delay(1000);
        while (PrintSerial.available())
        {

            a = PrintSerial.readStringUntil('\n'); // read the incoming data as string

            if (a.startsWith("SD printing byte "))
            { //Printing progress response

                a.remove(0, 17);
                if (debugmsg)
                {
                    Serial.println("Printing progress Triggered");
                }
                ind1;
                ind1 = a.indexOf('/');
                part1 = a.substring(0, ind1);
                part2 = a.substring(ind1 + 1);
                part1long = part1.toFloat();
                part2long = part2.toFloat();
                progress = (part1long / part2long) * 100;
            }
            else if (a.startsWith("ok T"))
            { //Temperature response
                if (debugmsg)
                {
                    Serial.println("Temperature response Triggered");
                }
                a.remove(0, 5);
                bedindex = a.indexOf(":");
                secondHalf = a.substring(bedindex, a.length());
                // Serial.println("secondHalf" + secondHalf);
                secondHalf.remove(0, 1);
                lastindex = secondHalf.indexOf(" ");
                secondHalf.remove(lastindex);
                extindex = a.indexOf(" ");
                firstHalf = a;
                firstHalf.remove(extindex);
                // a.remove(tempstring);
                if (debugmsg)
                {
                    Serial.println("Ext temp:" + firstHalf);
                    Serial.println("Bed temp:" + secondHalf);
                }
                exttemp = firstHalf;
                bedtemp = secondHalf;
            }
            else if (a.startsWith("echo"))
            {
                if (debugmsg)
                {
                    Serial.println("Elapsed Time Response Triggered");
                }
                lastind = a.lastIndexOf(":");
                a.remove(0, lastind);
                a.remove(0, 2);
                elapsedt = a;
            }
            else
            {
                if (debugmsg)
                {
                    Serial.println("Unknown response: " + a);
                }
            }
        }
        PrintSerial.print("M105\n"); //ok T:XXX.XX /XXX.XX B:XXX.XX /XXX.XX @:XXX B@:XXX
        if (debugmsg)
        {
            Serial.println("M105 Sent");
        }
        delay(1000);
        while (PrintSerial.available())
        {

            a = PrintSerial.readStringUntil('\n'); // read the incoming data as string

            if (a.startsWith("SD printing byte "))
            { //Printing progress response

                a.remove(0, 17);
                if (debugmsg)
                {
                    Serial.println("Printing progress Triggered");
                }
                ind1;
                ind1 = a.indexOf('/');
                part1 = a.substring(0, ind1);
                part2 = a.substring(ind1 + 1);
                part1long = part1.toFloat();
                part2long = part2.toFloat();
                progress = (part1long / part2long) * 100;
            }
            else if (a.startsWith("ok T"))
            { //Temperature response
                if (debugmsg)
                {
                    Serial.println("Temperature response Triggered");
                }
                a.remove(0, 5);
                bedindex = a.indexOf(":");
                secondHalf = a.substring(bedindex, a.length());
                // Serial.println("secondHalf" + secondHalf);
                secondHalf.remove(0, 1);
                lastindex = secondHalf.indexOf(" ");
                secondHalf.remove(lastindex);
                extindex = a.indexOf(" ");
                firstHalf = a;
                firstHalf.remove(extindex);
                // a.remove(tempstring);
                if (debugmsg)
                {
                    Serial.println("Ext temp:" + firstHalf);
                    Serial.println("Bed temp:" + secondHalf);
                }
                exttemp = firstHalf;
                bedtemp = secondHalf;
            }
            else if (a.startsWith("echo"))
            {
                if (debugmsg)
                {
                    Serial.println("Elapsed Time Response Triggered");
                }
                lastind = a.lastIndexOf(":");
                a.remove(0, lastind);
                a.remove(0, 2);
                elapsedt = a;
            }
            else
            {
                if (debugmsg)
                {
                    Serial.println("Unknown response: " + a);
                }
            }
        }

        static char json_response2[1024];
        char *p = json_response2;
        *p++ = '{';
        p += sprintf(p, "\"progress\":%.2f,", progress);
        p += sprintf(p, "\"exttemp\":%s,", exttemp.c_str());
        p += sprintf(p, "\"bedtemp\":%s,", bedtemp.c_str());
        p += sprintf(p, "\"elapsedt\":\"%s\"", elapsedt.c_str());
        *p++ = '}';
        *p++ = 0;
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        return httpd_resp_send(req, json_response2, strlen(json_response2));
    }
    else if (!strcmp(variable, "abort"))
    {
        aborted = val;
        if (aborted)
        {
            if (debugmsg)
            {
                Serial.println("Sending Abort Start");
            }
            // PrintSerial.print("G91; set coordinates to relative\n");
            // delay(2000);
            // PrintSerial.print("G1 F1800 E-3; retract\n");
            // delay(2000);
            // PrintSerial.print("G1 F3000 Z10; lift nozzle off the print 10mm\n");
            // delay(2000);
            // PrintSerial.print("G90; change to absolute\n");
            // delay(2000);
            // PrintSerial.print("G1 X0 Y220 F1000 ; prepare for part removal\n");
            // delay(2000);
            // PrintSerial.print("M106 S0 ; turn off cooling fan\n");
            // delay(2000);
            // PrintSerial.print("M104 S0 ; turn off extruder\n");
            // delay(2000);
            // PrintSerial.print("M140 S0 ; turn off bed\n");
            // delay(2000);
            // PrintSerial.print("M84 ; disable motors\n");
            PrintSerial.print(abortString);
            PrintSerial.print("M117 Heaters off, going HOME\n");
            if (debugmsg)
            {
                Serial.println("Sending Abort Complete");
            }
        }
        else
        {
        }
    }
    else if (!strcmp(variable, "shutdown"))
    {
        // shutdown = val;
        if (!shutdown)
        {
            digitalWrite(RELAY_PIN, HIGH);
            shutdown = 1;
        }
        else if (shutdown)
        {
            digitalWrite(RELAY_PIN, LOW);
            shutdown = 0;
        }
    }

    else if (!strcmp(variable, "command"))
    {
        String cmdText = value;
        cmdText = urldecode(cmdText);
        PrintSerial.print(cmdText);
        if (debugmsg){        
        Serial.print(cmdText + " Command Sent");
        }
        delay(2000);
        while (PrintSerial.available())
        {

            String cmdResponse = PrintSerial.readStringUntil('\n');
            static char json_response2[1024];
            char *p = json_response2;
            *p++ = '{';
            // p += sprintf(p, "\"progress\":%.2f,", progress);
            // p += sprintf(p, "\"exttemp\":%s,", exttemp.c_str());
            // p += sprintf(p, "\"bedtemp\":%s,", bedtemp.c_str());
            p += sprintf(p, "\"cmdresponse\":\"%s\"", cmdResponse.c_str());
            *p++ = '}';
            *p++ = 0;
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
            return httpd_resp_send(req, json_response2, strlen(json_response2));
        }

    }

    else
    {
        res = -1;
    }

    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
    if (debugmsg)
    {
        Serial.printf("Command issued");
    }
}

static esp_err_t status_handler(httpd_req_t *req)
{
    static char json_response[1024];

    sensor_t *s = esp_camera_sensor_get();
    char *p = json_response;
    *p++ = '{';
    p += sprintf(p, "\"temperature\":%f", (temprature_sens_read() - 32) / 1.8);
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
    if (debugmsg)
    {
        Serial.printf("Status sent");
    }
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    if (debugmsg)
    {
        Serial.printf("Index loading");
    }

    return httpd_resp_send(req, (const char *)INDEX2_HTML, strlen(INDEX2_HTML));
}

// void parseSerial(const String &fromSerial)
// {
//     String a, part1, part2, exttemp, bedtemp, elapsedt, firstHalf, secondHalf;
//     float progress, part1long, part2long;
//     int ind1, bedindex, lastindex, extindex, lastind;
//     a = fromSerial;

//     if (a.startsWith("SD printing byte "))
//     { //Printing progress response

//         a.remove(0, 17);
//         Serial.println("Printing progress Triggered");
//         ind1;
//         ind1 = a.indexOf('/');
//         part1 = a.substring(0, ind1);
//         part2 = a.substring(ind1 + 1);
//         part1long = part1.toFloat();
//         part2long = part2.toFloat();
//         progress = (part1long / part2long) * 100;
//     }
//     else if (a.startsWith("ok T"))
//     { //Temperature response
//         Serial.println("Temperature response Triggered");
//         a.remove(0, 5);
//         bedindex = a.indexOf(":");
//         secondHalf = a.substring(bedindex, a.length());
//         // Serial.println("secondHalf" + secondHalf);
//         secondHalf.remove(0, 1);
//         lastindex = secondHalf.indexOf(" ");
//         secondHalf.remove(lastindex);
//         extindex = a.indexOf(" ");
//         firstHalf = a;
//         firstHalf.remove(extindex);
//         // a.remove(tempstring);
//         Serial.println("Ext temp:" + firstHalf);
//         Serial.println("Bed temp:" + secondHalf);
//         exttemp = firstHalf;
//         bedtemp = secondHalf;
//     }
//     else if (a.startsWith("echo"))
//     {
//         Serial.println("Elapsed Time Response Triggered");
//         lastind = a.lastIndexOf(":");
//         a.remove(0, lastind);
//         a.remove(0, 2);
//         elapsedt = a;
//     }
//     else
//     {
//         Serial.println("Unknown response: " + a);
//     }
// }

void startCameraServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_uri_t status_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL};

    httpd_uri_t cmd_uri = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = cmd_handler,
        .user_ctx = NULL};

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;

    config.server_port = 9601; //stream port + also change this in the html-source in this file
                               // config.ctrl_port =8081;

    Serial.printf("Starting stream server on stream port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}

#include "OTA.h"
#include <WiFi.h>
#include <AsyncJson.h>
#include <Update.h>    
//#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "..\M3DUI\App\MenuApp.h"
#include "..\M3DUI\Components\M3DUI.h"
//#include "..\..\..\sd\cardreader.h"

WiFiClient wifiClient; 
String httpGETRequest(const char *serverName);
//String httpsGETRequest(const char *serverName);
String getHeaderValue(String header, String headerName);

void _TryOTAUpdate_();
//Check Firmware
bool inUpdate = false;
bool requestOtaUpdate = false;
bool otaLoopRunning = false;
TaskHandle_t otaLoopHandle;
void OTALoop(void*);

void TryOTAUpdate(){
    if (!otaLoopRunning){ 
        xTaskCreate(OTALoop, "ota", 4096, 0, 1, &otaLoopHandle);
    }
    if (inUpdate){ // dont put a double request
        return;
    }
    requestOtaUpdate = true; // put the request
}
void OTALoop(void*){    
    otaLoopRunning = true;       
    while(1){ // Can't exit a Task
        while(!requestOtaUpdate) // wait for request
            delay(100);
        requestOtaUpdate = false; // remove the flag for request
        inUpdate = true;
        _TryOTAUpdate_();
        inUpdate = false;
    }
}
void _TryOTAUpdate_()
{
    SERIAL_IMPL.println("TryOTAUpdate()");

    //Show Upgrade Screen
    if (menuHost.CurrentStep && menuHost.CurrentStep != &updateStep){
        SERIAL_IMPL.println("Switching to update step()");
        menuHost.CurrentStep->NextStep = &updateStep;
        menuHost.GotoNextStep(); // safe to be called during 
    }

    updateStep.NotifyOTAProgressChange(0);
    updateStep.NotifyOTAProgressChange("Finding Updates...");

    long contentLength = 0;
    bool isValidContentType = false;    
    WiFiClient wifiClient; 

    //Get Firmware JSON
    String httpData = httpGETRequest(FIRMWARE_INFO_URL);

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, httpData);

    if (error)
    {
        SERIAL_IMPL.print("deserializeJson() failed: ");
        // SERIAL_IMPL.println(error.f_str());
        updateStep.NotifyOTAProgressChange("Error finding update");
        return;
    }

    //Check Firmware Version
    int web_version = doc["version"]; // "2.0"
    SERIAL_IMPL.print("Web Version: ");
    SERIAL_IMPL.println(web_version);

    SERIAL_IMPL.print("CURRENT FIRMWARE: ");
    SERIAL_IMPL.println(FRIMWARE_VERSION);

    if ((float)FRIMWARE_VERSION >= web_version)
    {
        updateStep.NotifyOTAProgressChange("Already Up-to-date");
        return;
    }
    SERIAL_IMPL.println("Firmware: Ready to Update");

    updateStep.NotifyOTAProgressChange("Downloading...");
    //Execute OTA
    //wifiClient.setInsecure(); // or setCACert(root_ca)
    if (!wifiClient.connect(FIRMWARE_HOST, FIRMWARE_PORT))  // FIRMWARE_PORT = 443
    {
        // Connect to S3 failed
        // May be try?
        // Probably a choppy network?
        SERIAL_IMPL.printf("Connection to " FIRMWARE_HOST " failed. Please check your setup");
        // retry??
        // execOTA();
        updateStep.NotifyOTAProgressChange("Update Failed [1]");
        return;
    }
    // Connection Succeed.
    // Fecthing the bin
    SERIAL_IMPL.println("Fetching Bin: " FIRMWARE_BIN);

    // Get the contents of the bin file
    wifiClient.print(String("GET ") + FIRMWARE_PATH + " HTTP/1.1\r\n" +
                 "Host: " + FIRMWARE_HOST + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    updateStep.NotifyOTAProgressChange(26);

    unsigned long timeout = millis();
    while (wifiClient.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            SERIAL_IMPL.println("Client Timeout !");
            wifiClient.stop();
            updateStep.NotifyOTAProgressChange("Update failed [2]");
            return;
        }
    }

    //Connecting...
    while (wifiClient.available())
    {
        // read line till /n
        String line = wifiClient.readStringUntil('\n');
        // remove space, to check if the line is end of headers
        line.trim();

        // if the the line is empty,
        // this is end of headers
        // break the while and feed the
        // remaining `client` to the
        // Update.writeStream();
        if (!line.length())
        {
            //headers ended
            break; // and get the OTA started
        }

        // Check if the HTTP Response is 200
        // else break and Exit Update
        if (line.startsWith("HTTP/1.1"))
        {
            if (line.indexOf("200") < 0)
            {
                SERIAL_IMPL.println("Got a non 200 status code from server. Exiting OTA Update.");
                break;
            }
        }

        // extract headers here
        // Start with content length
        if (line.startsWith("Content-Length: "))
        {
            contentLength = getHeaderValue(line, "Content-Length: ").toInt();
            SERIAL_IMPL.printf("Got %d bytes from server\n", contentLength);
        }

        // Next, the content type
        if (line.startsWith("Content-Type: "))
        {
            String contentType = getHeaderValue(line, "Content-Type: ");
            SERIAL_IMPL.printf("Got %s payload\n", contentType.c_str());
            if (contentType == "application/octet-stream")
            {
                isValidContentType = true;
            }
        }
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    SERIAL_IMPL.printf("contentLength : %d, isValidContentType : %d\n", contentLength, isValidContentType);

    updateStep.NotifyOTAProgressChange(27);

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // BEGINNING OTA
        if (canBegin)
        {
            updateStep.NotifyOTAProgressChange(30);
            updateStep.NotifyOTAProgressChange("Downloading firmware");
            SERIAL_IMPL.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the SERIAL_IMPL monitor
            // So be patient. This may take 2 - 5mins to complete
            //size_t written = Update.writeStream(wifiClient);
            long totalToWrite = contentLength;
            long perPacket = 20000;
            long written = 0;
            SERIAL_IMPL.printf("Total to write: %i\n", totalToWrite);
            SERIAL_IMPL.printf("Per packet: %i\n", perPacket);

            uint8_t* buffer = new uint8_t[perPacket];
            while(written < totalToWrite){
                int thisPacket = perPacket;
                if ((totalToWrite - written) < perPacket)
                    thisPacket = (totalToWrite - written);
                thisPacket = wifiClient.read(buffer, thisPacket);
                if (thisPacket <= 0) {
                    delay(100); // otherwise the wifi client doesn't read too much
                    continue;
                }

                Update.write(buffer, thisPacket);   
                SERIAL_IMPL.printf("Dumped: %i\n", thisPacket);

                updateStep.NotifyOTAProgressChange((written * 65) / totalToWrite + 30);
                SERIAL_IMPL.printf("Progress: %d\n", (written * 65) / totalToWrite + 30);
                written += thisPacket;
                delay(100); // otherwise the wifi client doesn't read too much
            }
            delete buffer;

            if (written == contentLength)
            {
                SERIAL_IMPL.printf("Written : %d successfully\n", written);
            }
            else
            {
                SERIAL_IMPL.printf("Written only : %d/%d, Retry?\n", written, contentLength);
                // retry??
                // execOTA();
                updateStep.NotifyOTAProgressChange("Update failed [3]");
                return;
            }

            if (Update.end())
            {
                SERIAL_IMPL.println("OTA done!");
                updateStep.NotifyOTAProgressChange(100);
                if (Update.isFinished())
                {
                    SERIAL_IMPL.println("Update successfully completed. Rebooting.");                    
                    for (int i = 5; i > 0; i--){
                        delay(1000);                        
                        updateStep.NotifyOTAProgressChange(String("Update complete (") + String(i) + String(")"));
                    }
                    ESP.restart();
                }
                else
                {
                    updateStep.NotifyOTAProgressChange("Update failed [4]");
                    SERIAL_IMPL.println("Update not finished? Something went wrong!");
                }
            }
            else
            {
                updateStep.NotifyOTAProgressChange("Update failed [5]");
                SERIAL_IMPL.printf("Error Occurred. Error #: %d\n", Update.getError());
            }
        }
        else
        {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            SERIAL_IMPL.println("Not enough space to begin OTA");
            updateStep.NotifyOTAProgressChange("Update failed [6]");
            wifiClient.flush();
        }
    }
    else
    {
        updateStep.NotifyOTAProgressChange("Update failed [7]");
        SERIAL_IMPL.println("There was no content in the response");
        wifiClient.flush();
        updateStep.NotifyOTAProgressChange("Update failed [8]");
    }
}

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName)
{
    return header.substring(strlen(headerName.c_str()));
}
// String httpsGETRequest(const char *serverName)
// {
//     // Step 1: Check Wi-Fi connection
//     if (WiFi.status() != WL_CONNECTED) {
//         SERIAL_IMPL.println("[DEBUG] WiFi not connected!");
//         SERIAL_IMPL.print("[DEBUG] WiFi status: ");
//         SERIAL_IMPL.println(WiFi.status());
//         return "";
//     }
//     SERIAL_IMPL.print("[DEBUG] Connected to WiFi, IP: ");
//     SERIAL_IMPL.println(WiFi.localIP().toString().c_str());

//     // Step 2: Try DNS resolution
//     String host = serverName;
//     // Extract hostname from URL (e.g., "https://example.com/path" → "example.com")
//     int idx1 = host.indexOf("://");
//     if (idx1 > 0) host = host.substring(idx1 + 3);
//     int idx2 = host.indexOf('/');
//     if (idx2 > 0) host = host.substring(0, idx2);

//     IPAddress resolvedIP;
//     if (WiFi.hostByName(host.c_str(), resolvedIP)) {
//         SERIAL_IMPL.print("[DEBUG] DNS resolved ");
//         SERIAL_IMPL.print(host.c_str());
//         SERIAL_IMPL.print(" -> ");
//         SERIAL_IMPL.println(resolvedIP.toString().c_str());
//     } else {
//         SERIAL_IMPL.print("[DEBUG] DNS resolution failed for: ");
//         SERIAL_IMPL.println(host.c_str());
//         return "";
//     }

//     // Step 3: HTTPS request
//     WiFiClientSecure client;
//     client.setInsecure(); // quick way to skip SSL cert check (not for production)

//     HTTPClient http;
//     SERIAL_IMPL.print("[DEBUG] Beginning HTTPS request to: ");
//     SERIAL_IMPL.println(serverName);

//     if (!http.begin(client, serverName)) {
//         SERIAL_IMPL.println("[DEBUG] http.begin() failed!");
//         return "";
//     }

//     int httpResponseCode = http.GET();

//     String payload = "";

//     if (httpResponseCode > 0)
//     {
//         SERIAL_IMPL.print("[DEBUG] HTTP Response code: ");
//         SERIAL_IMPL.println(httpResponseCode);
//         payload = http.getString();
//     }
//     else
//     {
//         SERIAL_IMPL.print("[DEBUG] HTTP GET failed, error code: ");
//         SERIAL_IMPL.println(httpResponseCode);

//         // Optional: interpret common error codes
//         switch (httpResponseCode) {
//             case HTTPC_ERROR_CONNECTION_REFUSED:
//                 SERIAL_IMPL.println("[DEBUG] Connection refused by server");
//                 break;
//             case HTTPC_ERROR_SEND_HEADER_FAILED:
//                 SERIAL_IMPL.println("[DEBUG] Send header failed");
//                 break;
//             case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
//                 SERIAL_IMPL.println("[DEBUG] Send payload failed");
//                 break;
//             case HTTPC_ERROR_NOT_CONNECTED:
//                 SERIAL_IMPL.println("[DEBUG] Not connected");
//                 break;
//             case HTTPC_ERROR_CONNECTION_LOST:
//                 SERIAL_IMPL.println("[DEBUG] Connection lost");
//                 break;
//             case HTTPC_ERROR_NO_STREAM:
//                 SERIAL_IMPL.println("[DEBUG] No stream");
//                 break;
//             case HTTPC_ERROR_NO_HTTP_SERVER:
//                 SERIAL_IMPL.println("[DEBUG] No HTTP server");
//                 break;
//             default:
//                 SERIAL_IMPL.println("[DEBUG] Unknown error");
//                 break;
//         }
//     }

//     http.end();
//     return payload;
// }

//HTTP - GET
String httpGETRequest(const char *serverName)
{
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    String payload = "";

    if (httpResponseCode > 0)
    {
        SERIAL_IMPL.print("HTTP Response code: ");
        SERIAL_IMPL.println(httpResponseCode);
        payload = http.getString();
    }
    else
    {
        SERIAL_IMPL.print("HTTP Get Error code: ");
        SERIAL_IMPL.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}

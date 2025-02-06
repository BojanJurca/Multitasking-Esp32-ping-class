#include "ESP32_ping.hpp"


void setup () {
    Serial.begin (115200);
    while (!Serial) 
        delay (10);
    delay (1000);

    WiFi.begin ("YourSSID","YourPassword"); // use your WiFi credentials

    Serial.print ("\nConnecting ");
    while (WiFi.status () != WL_CONNECTED) {
        delay (500);
        Serial.print (".");
    }
    Serial.print ("\nConnected, STAtion IP: ");
    Serial.print (WiFi.localIP ());
    Serial.print(", router's IP: ");
    Serial.println (WiFi.gatewayIP ());


    // example 1: simple router pinging
    Serial.println ();
    esp32_ping simplePing;

    Serial.printf ("Please wait ...\n");
    simplePing.ping (WiFi.gatewayIP ().toString ().c_str ());
    if (simplePing.errText () != NULL) {
        Serial.printf ("Error %%\n", simplePing.errText ());
    } else {
        Serial.printf ("Ping statistics for %s:\n"
                      "    Packets: Sent = %i, Received = %i, Lost = %i", simplePing.target (), simplePing.sent (), simplePing.received (), simplePing.lost ());
        if (simplePing.sent ()) {
            Serial.printf (" (%.2f%% loss)\nRound trip:\n"
                          "   Min = %.3fms, Max = %.3fms, Avg = %.3fms, Stdev = %.3fms\n", (float) simplePing.lost () / (float) simplePing.sent () * 100, simplePing.min_time (), simplePing.max_time (), simplePing.mean_time (), sqrt (simplePing.var_time () / simplePing.received ()));
        } else {
            Serial.printf ("\n");
        }
    }


    // example 2: ping and capture intermentidate results by overloading onReceive function
    Serial.println ();
    class capture_ping : public esp32_ping {
        void onReceive (int bytes) {
            if (elapsed_time ())
                Serial.printf ("Reply from %s: bytes = %i time = %.3fms\n", target (), size (), elapsed_time ());
            else
                Serial.printf ("Reply from %s: time-out\n", target ());
        }
    };
    capture_ping capturePing;

    capturePing.ping ("github.com");
    if (capturePing.errText () != NULL) {
        Serial.printf ("Error %%\n", capturePing.errText ());
    } else {
        Serial.printf ("Ping statistics for %s:\n"
                      "    Packets: Sent = %i, Received = %i, Lost = %i", capturePing.target (), capturePing.sent (), capturePing.received (), capturePing.lost ());
        if (capturePing.sent()) {
            Serial.printf (" (%.2f%% loss)\nRound trip:\n"
                          "   Min = %.3fms, Max = %.3fms, Avg = %.3fms, Stdev = %.3fms\n", (float) capturePing.lost () / (float) capturePing.sent () * 100, capturePing.min_time (), capturePing.max_time (), capturePing.mean_time (), sqrt (capturePing.var_time () / capturePing.received ()));
        } else {
            Serial.printf ("\n");
        }
    }


    // example 3: simultaneous pinging from different tasks
    Serial.println ();
    capture_ping githubPing;

                xTaskCreate ([] (void *param)  {
                    delay (3500);
                    capture_ping routerPing;
                    routerPing.ping (WiFi.gatewayIP ().toString ().c_str (), 10);
                    if (routerPing.errText () != NULL) {
                        Serial.printf ("%s\n", routerPing.errText ());
                    } else {
                        Serial.printf ("Ping statistics for %s:\n"
                                      "    Packets: Sent = %i, Received = %i, Lost = %i", routerPing.target (), routerPing.sent (), routerPing.received (), routerPing.lost ());
                        if (routerPing.sent()) {
                            Serial.printf (" (%.2f%% loss)\nRound trip:\n"
                                          "   Min = %.3fms, Max = %.3fms, Avg = %.3fms, Stdev = %.3fms\n", (float) routerPing.lost () / (float) routerPing.sent () * 100, routerPing.min_time (), routerPing.max_time (), routerPing.mean_time (), sqrt (routerPing.var_time () / routerPing.received ()));
                        } else {
                            Serial.printf ("\n");
                        }
                    }
                    vTaskDelete (NULL);
                }, 
                "ping_task", 4068, NULL, 1, NULL);


    githubPing.ping ("github.com", 10);
    if (githubPing.errText () != NULL) {
        Serial.printf ("Error %%\n", githubPing.errText ());
    } else {
        Serial.printf ("Ping statistics for %s:\n"
                      "    Packets: Sent = %i, Received = %i, Lost = %i", githubPing.target (), githubPing.sent (), githubPing.received (), githubPing.lost ());
        if (githubPing.sent ()) {
            Serial.printf (" (%.2f%% loss)\nRound trip:\n"
                          "   Min = %.3fms, Max = %.3fms, Avg = %.3fms, Stdev = %.3fms\n", (float) githubPing.lost () / (float) githubPing.sent () * 100, githubPing.min_time (), githubPing.max_time (), githubPing.mean_time (), sqrt (githubPing.var_time () / githubPing.received ()));
        } else {
            Serial.printf ("\n");
        }
    }

}

void loop () {
  
}

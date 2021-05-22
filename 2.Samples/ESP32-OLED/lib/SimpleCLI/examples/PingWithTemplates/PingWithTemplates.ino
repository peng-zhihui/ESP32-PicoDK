/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/SimpleCLI
 */

// Include Library
#include <SimpleCLI.h>

// Create CLI Object
SimpleCLI cli;

// Commands
Command ping;

// Callback function for ping command
void pingCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get arguments
    Argument numberArg = cmd.getArgument("number");
    Argument strArg    = cmd.getArgument("str");

    // Get values
    int numberValue = numberArg.getValue().toInt();
    String strValue = strArg.getValue();

    // Print response
    for (int i = 0; i<numberValue; i++) Serial.println(strValue);
}

// Callback in case of an error
void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        Serial.print(cmdError.getCommand().toString());
        Serial.println("\"?");
    }
}

void setup() {
    // Startup stuff
    Serial.begin(9600);
    delay(2000);
    Serial.println("Started!");

    cli.setOnError(errorCallback); // Set error Callback

    // Create the ping command with callback function
    // p, ping, pong will be accepted as a valid name
    ping = cli.addCmd("p/ing,pong", pingCallback);

    // Add required argument with name "number"
    // -num and -number will be accepted as a valid name
    ping.addArg("num/ber");

    // Add optional argument with name "str" and default value "pong"
    ping.addArg("str/ing", "pong");

    // Possible command calls:
    // ping -number 1
    // ping -num 1
    // ping -str "Hello World" -number 1
    // ping -str "Hello World" -num 1
    // ping -string "Hello World" -number 1
    // ping -string "Hello World" -num 1
    // ping -number 1 -str "Hello World"
    // ping -number 1 -string "Hello World"
    // ping -num 1 -str "Hello World"
    // ping -num 1 -string "Hello World"

    Serial.println("Type: ping -str \"Hello World\" -number 1");
}

void loop() {
    // Check if user typed something into the serial monitor
    if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

        // Echo the user input
        Serial.print("# ");
        Serial.println(input);

        // Parse the user input into the CLI
        cli.parse(input);
    }

    if (cli.errored()) {
        CommandError cmdError = cli.getError();

        Serial.print("ERROR: ");
        Serial.println(cmdError.toString());

        if (cmdError.hasCommand()) {
            Serial.print("Did you mean \"");
            Serial.print(cmdError.getCommand().toString());
            Serial.println("\"?");
        }
    }
}
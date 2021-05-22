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

    Serial.println("Pong!");
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
    // Enable Serial connection at given baud rate
    Serial.begin(9600);

    // Wait a bit until device is started
    delay(2000);

    // Print something to let us know that everything is working so far
    Serial.println("Hello World!");

    // Create the ping command with callback function
    ping = cli.addCmd("ping", pingCallback);

    // [Optional] Check if our command was successfully added
    if (!ping) {
        Serial.println("Something went wrong :(");
    } else {
        Serial.println("Ping was added to the CLI!");
    }

    // Set error Callback
    cli.setOnError(errorCallback);

    // Start the loop
    Serial.println("Type ping to see what happens!");
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
}
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

void setup() {
    // Enable Serial connection at given baud rate
    Serial.begin(9600);

    // Wait a bit until device is started
    delay(2000);

    // Print something to let us know that everything is working so far
    Serial.println("Hello World!");

    // Create the ping command
    ping = cli.addCmd("ping");

    // [Optional] Check if our command was successfully added
    if (!ping) {
        Serial.println("Something went wrong :(");
    } else {
        Serial.println("Ping was added to the CLI!");
    }

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

    // Check for newly parsed commands
    if (cli.available()) {
        // Get command out of queue
        Command cmd = cli.getCmd();

        // React on our ping command
        if (cmd == ping) {
            Serial.println("Pong!");
        }
    }

    // Check for parsing errors
    if (cli.errored()) {
        // Get error out of queue
        CommandError cmdError = cli.getError();

        // Print the error
        Serial.print("ERROR: ");
        Serial.println(cmdError.toString());

        // Print correct command structure
        if (cmdError.hasCommand()) {
            Serial.print("Did you mean \"");
            Serial.print(cmdError.getCommand().toString());
            Serial.println("\"?");
        }
    }
}
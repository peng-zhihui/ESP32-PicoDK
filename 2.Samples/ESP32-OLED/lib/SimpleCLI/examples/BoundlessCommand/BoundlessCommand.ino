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
Command sum;

// Callback function for sum command
void sumCallback(cmd* c) {
    Command cmd(c);               // Create wrapper object

    int argNum = cmd.countArgs(); // Get number of arguments
    int sum    = 0;

    // Go through all arguments and add their value up
    for (int i = 0; i<argNum; i++) {
        Argument arg      = cmd.getArg(i);
        String   argValue = arg.getValue();
        int argIntValue   = argValue.toInt();

        if (argIntValue > 0) {
            if (i > 0) Serial.print('+');
            Serial.print(argIntValue);

            sum += argIntValue;
        }
    }

    // Print result
    Serial.print(" = ");
    Serial.println(sum);
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


    // Create the sum command, accepting infinite number of arguments
    // Each argument is seperated by a space
    // For example: sum 1 2 3
    sum = cli.addBoundlessCommand("sum", sumCallback);


    Serial.println("Type: sum 1 2 3");
}

void loop() {
    // Check if user typed something into the serial monitor
    if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

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
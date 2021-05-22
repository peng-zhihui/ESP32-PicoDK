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
    Argument cArg      = cmd.getArgument("c");

    // Get values
    int numberValue = numberArg.getValue().toInt();
    String strValue = strArg.getValue();
    bool   cValue   = cArg.isSet();

    if (cValue) strValue.toUpperCase();

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
    ping = cli.addCommand("ping", pingCallback);

    // Add argument with name "number", the value has to be set by the user
    // for example: echo -number 1
    ping.addArgument("number");

    // Add argument with name "str", with value "pong" in case the user didn't specify it
    // Positional arguments do not need to be called by their name
    // for example: echo -str "pong"
    //              echo "pong"
    ping.addPositionalArgument("str", "pong");

    // Add flag argument with name "c", which can be set or not, but does not take any value after the name
    // for example: echo -c
    ping.addFlagArgument("c");

    // Possible command calls:
    // ping -number 1 -str "pong" -c
    // ping -number 1 -str "pong"
    // ping -number 1
    // ping -number 1 -c
    // ping -number 1 "pong" -c
    // ping -number 1 -c "pong"
    // ping -c -number 1 "pong"
    // ping -c "pong" -number 1
    // ping "pong" -number 1
    // ping -number 1 "pong"

    // Further Explaination:
    // -number is required to be set, since it does not have a default value
    // -str "pong" can also be "pong", because it's the only positional arguments
    // If there was another positional argument, you have to write them in the correct order!
    // -c can be used in the command or not, it's a flag (like a boolean), it's there (true) or it's not (false)

    Serial.println("Type: ping -str \"Hello World\" -number 1 -c");
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
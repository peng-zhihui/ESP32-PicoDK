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
Command cowsay;

// Callback function for cowsay command
void cowsayCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    // Get value of argument
    String argVal = arg.getValue();

    // Print Value
    Serial.print(' ');

    for (int i = 0; i<argVal.length(); i++) Serial.print('_');
    Serial.println(' ');

    Serial.print("< ");
    Serial.print(argVal);
    Serial.println(" >");

    Serial.print(' ');

    for (int i = 0; i<argVal.length(); i++) Serial.print('-');
    Serial.println(' ');

    Serial.println("        \\   ^__^");
    Serial.println("         \\  (oo)\\_______");
    Serial.println("            (__)\\       )\\/\\");
    Serial.println("                ||----w |");
    Serial.println("                ||     ||");
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


    // Create the cowsay command with callback function
    // A single argument command has only one argument
    // in which the complete string is saved, that comes after the command name.
    // Spaces, which usually seperate arguments, will be ignored and any value will be accespted as valid.
    // For example: cowsay "Hello there!"
    //              cowsays Hello there!
    cowsay = cli.addSingleArgCmd("cowsay", cowsayCallback);


    Serial.println("Type: cowsay something");
}

void loop() {
    // Check if user typed something into the serial monitor
    if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

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
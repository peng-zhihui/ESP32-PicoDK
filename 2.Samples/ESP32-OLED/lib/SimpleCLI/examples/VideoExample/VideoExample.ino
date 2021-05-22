/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/SimpleCLI
 */

// Watch the tutorial here: https://youtu.be/UyW-wICdnKo

 #include <SimpleCLI.h>

SimpleCLI cli;

Command cmdPing;
Command cmdPong;

void errorCallback(cmd_error* errorPtr) {
    CommandError e(errorPtr);

    Serial.println("ERROR: " + e.toString());

    if (e.hasCommand()) {
        Serial.println("Did you mean? " + e.getCommand().toString());
    } else {
        Serial.println(cli.toString());
    }
}

void pongCallback(cmd* cmdPtr) {
    Command cmd(cmdPtr);

    int argNum = cmd.countArgs();

    for (int i = 0; i < argNum; i++) {
        Serial.println(cmd.getArgument(i).getValue());
    }
}

void pingCallback(cmd* cmdPtr) {
    Command cmd(cmdPtr);

    Argument argN   = cmd.getArgument("num");
    String   argVal = argN.getValue();
    int n           = argVal.toInt();

    Argument argStr = cmd.getArgument("str");
    String   strVal = argStr.getValue();

    Argument argC = cmd.getArgument("c");
    bool     c    = argC.isSet();

    if (c) strVal.toUpperCase();

    for (int i = 0; i < n; i++) {
        Serial.println(strVal);
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Hello World");

    cmdPing = cli.addCommand("ping", pingCallback);
    cmdPing.addPositionalArgument("str", "pong");
    cmdPing.addArgument("n/um/ber,anzahl", "1");
    cmdPing.addFlagArgument("c");

    cmdPong = cli.addBoundlessCommand("pong,hello", pongCallback);

    cli.setOnError(errorCallback);
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        Serial.println("# " + input);

        cli.parse(input);
    }
}
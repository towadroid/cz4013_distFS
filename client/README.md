# Client application

## Build from source
The application can be built form source by executing the following commands:
```bash
cd client
mkdir bin
javac -d bin src/Exceptions/* src/Helpers/* src/Services/* src/Main.java
```

### Dependencies
* java and javac ≥ 10.0.2

## Usage
The server can then be run with `java Main ip port at_most failure_rate freshness`.
See the report for further explanation.

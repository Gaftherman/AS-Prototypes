// file_io_test.as - Demonstrates scriptfile add-on

void main() {
    println("--- AngelScript File I/O Test ---");

    file f;
    string filename = "test_output.txt";

    // Write file
    if (f.open(filename, "w") >= 0) {
        f.writeString("Hello World from AngelScript File I/O!\n");
        f.writeString("This file was written automatically by file_io_test.as.\n");
        f.close();
        println("Successfully wrote to " + filename);
    } else {
        println("Failed to open file for writing.");
    }

    // Read file back
    if (f.open(filename, "r") >= 0) {
        println("\nReading contents of " + filename + ":");
        string line;
        while (!f.isEndOfFile()) {
            line = f.readLine();
            println(" > " + line);
        }
        f.close();
    } else {
        println("Failed to open file for reading.");
    }
}

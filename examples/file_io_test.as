// file_io_test.as - Demonstrates scriptfile add-on

void main() {
    Console::WriteLine("--- AngelScript File I/O Test ---");

    file f;
    string filename = "test_output.txt";

    // Write file
    if (f.open(filename, "w") >= 0) {
        f.writeString("Hello World from AngelScript File I/O!\n");
        f.writeString("This file was written automatically by file_io_test.as.\n");
        f.close();
        Console::WriteLine("Successfully wrote to " + filename);
    } else {
        Console::WriteLine("Failed to open file for writing.");
    }

    // Read file back
    if (f.open(filename, "r") >= 0) {
        Console::WriteLine("\nReading contents of " + filename + ":");
        string line;
        while (!f.isEndOfFile()) {
            line = f.readLine();
            Console::WriteLine(" > " + line);
        }
        f.close();
    } else {
        Console::WriteLine("Failed to open file for reading.");
    }
}

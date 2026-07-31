// array_dict_test.as - Demonstrates scriptarray and scriptdictionary add-ons

void main() {
    Console::PrintLine("--- AngelScript Array & Dictionary Test ---");

    // Dynamic Array Test
    array<string> fruits = {"Apple", "Banana", "Cherry", "Dragonfruit"};
    Console::PrintLine("Fruits count: " + fruits.length());
    for (uint i = 0; i < fruits.length(); ++i) {
        Console::PrintLine("  [" + i + "] " + fruits[i]);
    }

    // Dictionary Test
    dictionary scores;
    scores.set("Alice", 95);
    scores.set("Bob", 88);
    scores.set("Charlie", 92);

    int aliceScore = 0;
    scores.get("Alice", aliceScore);
    Console::PrintLine("Alice's Score: " + aliceScore);
}

// array_dict_test.as - Demonstrates scriptarray and scriptdictionary add-ons

void main() {
    println("--- AngelScript Array & Dictionary Test ---");

    // Dynamic Array Test
    array<string> fruits = {"Apple", "Banana", "Cherry", "Dragonfruit"};
    println("Fruits count: " + fruits.length());
    for (uint i = 0; i < fruits.length(); ++i) {
        println("  [" + i + "] " + fruits[i]);
    }

    // Dictionary Test
    dictionary scores;
    scores.set("Alice", 95);
    scores.set("Bob", 88);
    scores.set("Charlie", 92);

    int aliceScore = 0;
    scores.get("Alice", aliceScore);
    println("Alice's Score: " + aliceScore);
}

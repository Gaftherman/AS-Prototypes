// test_math.as - Math test script for doctest runner

void main() {
    println("Executing test_math.as...");
    float val = sin(1.570795f);
    if (abs(val - 1.0f) < 0.01f) {
        Tests::Passes++;
    } else {
        Tests::Fails++;
    }
}

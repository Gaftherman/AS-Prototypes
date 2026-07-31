// test_math.as - Math test script for doctest runner

void main() {
    Console::PrintLine("Executing test_math.as...");
    float val = sin(1.570795f);
    Tests::Expect( "Exception catch", true, ( abs(val - 1.0f) < 0.01f ) );
}

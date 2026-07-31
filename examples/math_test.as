// math_test.as - Demonstrates scriptmath add-on

void main() {
    Console::PrintLine("--- AngelScript Math Add-on Test ---");

    float val = 3.14159265f;
    float sinVal = sin(val / 2.0f);
    float cosVal = cos(val / 2.0f);
    float sqrVal = sqrt(16.0f);
    float powVal = pow(2.0f, 10.0f);

    Console::PrintLine("pi / 2 = " + (val / 2.0f));
    Console::PrintLine("sin(pi / 2) = " + sinVal);
    Console::PrintLine("cos(pi / 2) = " + cosVal);
    Console::PrintLine("sqrt(16.0)  = " + sqrVal);
    Console::PrintLine("pow(2, 10)   = " + powVal);
}

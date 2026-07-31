// math_test.as - Demonstrates scriptmath add-on

void main() {
    Console::WriteLine("--- AngelScript Math Add-on Test ---");

    float val = 3.14159265f;
    float sinVal = sin(val / 2.0f);
    float cosVal = cos(val / 2.0f);
    float sqrVal = sqrt(16.0f);
    float powVal = pow(2.0f, 10.0f);

    Console::WriteLine("pi / 2 = " + (val / 2.0f));
    Console::WriteLine("sin(pi / 2) = " + sinVal);
    Console::WriteLine("cos(pi / 2) = " + cosVal);
    Console::WriteLine("sqrt(16.0)  = " + sqrVal);
    Console::WriteLine("pow(2, 10)   = " + powVal);
}

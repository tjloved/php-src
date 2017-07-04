--TEST--
Error cases of compound shift assignment on strings
--FILE--
<?php

function fn136079666()
{
    $n = "65";
    $n <<= $n;
    var_dump($n);
    $n = "-1";
    try {
        $n <<= $n;
        var_dump($n);
    } catch (ArithmeticError $e) {
        echo "\nException: " . $e->getMessage() . "\n";
    }
    $n = "65";
    $n >>= $n;
    var_dump($n);
    $n = "-1";
    try {
        $n >>= $n;
        var_dump($n);
    } catch (ArithmeticError $e) {
        echo "\nException: " . $e->getMessage() . "\n";
    }
    $n = "0";
    try {
        $n %= $n;
        var_dump($n);
    } catch (DivisionByZeroError $e) {
        echo "\nException: " . $e->getMessage() . "\n";
    }
    $n = "-1";
    $n %= $n;
    var_dump($n);
}
fn136079666();
--EXPECTF--
int(0)

Exception: Bit shift by negative number
int(0)

Exception: Bit shift by negative number

Exception: Modulo by zero
int(0)

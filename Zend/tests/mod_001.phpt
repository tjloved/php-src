--TEST--
modulus by zero
--FILE--
<?php

function fn313261348()
{
    $a = array(1, 2, 3);
    $b = array();
    try {
        $c = $a % $b;
        var_dump($c);
    } catch (DivisionByZeroError $e) {
        echo "Exception: " . $e->getMessage() . "\n";
    }
    echo "Done\n";
}
fn313261348();
--EXPECT--
Exception: Modulo by zero
Done

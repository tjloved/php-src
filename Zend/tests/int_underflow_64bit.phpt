--TEST--
testing integer underflow (64bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

function fn1834869986()
{
    $doubles = array(-9.223372036854776E+18, -9.223372036854776E+18, -9.223372036854776E+18, -9.223372036854776E+18, -9.223372036854776E+18);
    foreach ($doubles as $d) {
        $l = (int) $d;
        var_dump($l);
    }
    echo "Done\n";
}
fn1834869986();
--EXPECTF--
int(-9223372036854775808)
int(-9223372036854775808)
int(-9223372036854775808)
int(-9223372036854775808)
int(-9223372036854775808)
Done

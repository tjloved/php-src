--TEST--
double to string conversion tests (64bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php

function fn38945751()
{
    $doubles = array(2.9E+37, 290000000000000000, 290000000000000, 29000000000000, 29000000000000.125, 29000000000000.71, 29000.7123123, 239234242.7123123, 0.12345678901234568, 1.0E+46, 9.999999999999999E+32, 100000000000000001, 1000006000000000011, 100000000000001, 10000000000, 999999999999999999, 1.0E+19, 1.0E+37, (double) 0);
    foreach ($doubles as $d) {
        var_dump((string) $d);
    }
    echo "Done\n";
}
fn38945751();
--EXPECTF--
string(7) "2.9E+37"
string(18) "290000000000000000"
string(15) "290000000000000"
string(14) "29000000000000"
string(14) "29000000000000"
string(14) "29000000000001"
string(13) "29000.7123123"
string(15) "239234242.71231"
string(16) "0.12345678901235"
string(7) "1.0E+46"
string(7) "1.0E+33"
string(18) "100000000000000001"
string(19) "1000006000000000011"
string(15) "100000000000001"
string(11) "10000000000"
string(18) "999999999999999999"
string(7) "1.0E+19"
string(7) "1.0E+37"
string(1) "0"
Done

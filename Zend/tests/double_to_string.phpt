--TEST--
double to string conversion tests
--INI--
precision=14
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

function fn893471860()
{
    $doubles = array(290000000000000000, 290000000000000, 29000000000000, 29000000000000.125, 29000000000000.71, 29000.7123123, 239234242.7123123, 0.12345678901234568, 10000000000000, 100000000000000, 1000000000000000001, 100000000000001, 10000000000, 999999999999999, 9999999999999999, (double) 0);
    foreach ($doubles as $d) {
        var_dump((string) $d);
    }
    echo "Done\n";
}
fn893471860();
--EXPECTF--	
string(7) "2.9E+17"
string(7) "2.9E+14"
string(%d) "2%s"
string(%d) "2%s"
string(%d) "29%d"
string(13) "29000.7123123"
string(15) "239234242.71231"
string(16) "0.12345678901235"
string(14) "10000000000000"
string(7) "1.0E+14"
string(7) "1.0E+18"
string(7) "1.0E+14"
string(11) "10000000000"
string(7) "1.0E+15"
string(7) "1.0E+16"
string(1) "0"
Done

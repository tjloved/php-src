--TEST--
dividing doubles
--INI--
precision=14
--FILE--
<?php

function fn2123680909()
{
    $d1 = 1.1;
    $d2 = 434234.234;
    $c = $d2 / $d1;
    var_dump($c);
    $d1 = 1.1;
    $d2 = "434234.234";
    $c = $d2 / $d1;
    var_dump($c);
    $d1 = "1.1";
    $d2 = "434234.234";
    $c = $d2 / $d1;
    var_dump($c);
    echo "Done\n";
}
fn2123680909();
--EXPECTF--	
float(394758.39454545)
float(394758.39454545)
float(394758.39454545)
Done
